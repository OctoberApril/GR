#include "MeshRenderer.h"

#include <iostream>

#include "d3dx12.h"
#include "GameObject.h"
#include "Material.h"
#include "Graphics.h"
#include "Mesh.h"
#include "ShaderUniform.h"
#include "UploadBuffer.h"

MeshRenderer::MeshRenderer() :m_Material(nullptr), m_CommandAllocator(nullptr), m_CommandList(nullptr), m_Device(nullptr), m_bInit(false),m_UploadBuffer(nullptr)
{

}

void MeshRenderer::Initialize()
{
	m_Device = DX12Graphics::Instance->GetDevice();
	assert(m_Device != nullptr);
	m_UploadBuffer = new UploadBuffer();

	int id = m_Material->GetParameterIndex("HHQ_MATRIX_Model");
	if (id != -1)
	{
		RootSignatureTableBindItem item = {};
		item.ParameterIndex = id;
		item.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;

		ComPtr<ID3D12Resource> resource;
		auto heapPropertiesDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(glm::mat4));
		m_Device->CreateCommittedResource(&heapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(resource.GetAddressOf()));		
		item.BufferLocation = resource->GetGPUVirtualAddress();

		m_LocalUniformBindMap.emplace("HHQ_MATRIX_Model", item);
		m_LocalUniformBufferMap.emplace("HHQ_MATRIX_Model", resource);
	}
	
	m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAllocator.GetAddressOf()));
	m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(m_CommandList.GetAddressOf()));
	m_CommandList->Close();

	//初始化VertexBufferView和IndexBufferView 绑定static资源
	Mesh* mesh = this->m_pGameObject->GetComponent<Mesh>();
	assert(mesh != nullptr);
	if(!mesh->IsEmpty())
	{		
		auto vertices = mesh->GetVertices();
		auto uvs = mesh->GetUv0s();		
		auto indices = mesh->GetIndices();

		struct tempVertexData
		{
			glm::vec3 vertex;
			glm::vec2 uv;
		};
		
		std::vector<tempVertexData> data;
		for(int i = 0;i < vertices.size();i++)
		{
			tempVertexData td;
			td.vertex = vertices[i];
			td.uv = uvs[i];
			data.push_back(td);
		}
		
		BufferAllocation verticesBuffer = m_UploadBuffer->Allocate(data.size() * sizeof(tempVertexData), 4);
		BufferAllocation indicesBuffer = m_UploadBuffer->Allocate(indices.size() * sizeof(uint32_t), 4);

		memcpy(verticesBuffer.CpuAddress, &data[0], data.size() * sizeof(tempVertexData));
		m_VertexBufferView.BufferLocation = verticesBuffer.GpuAddress;
		m_VertexBufferView.SizeInBytes = verticesBuffer.Size;
		m_VertexBufferView.StrideInBytes = sizeof(tempVertexData);

		memcpy(indicesBuffer.CpuAddress, &indices[0], indices.size() * sizeof(uint32_t));
		m_IndexBufferView.BufferLocation = indicesBuffer.GpuAddress;
		m_IndexBufferView.SizeInBytes = indicesBuffer.Size;
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	}
	
	m_bInit = true;
}

bool MeshRenderer::IsNeedToDraw() const
{
	Mesh* mesh = this->m_pGameObject->GetComponent<Mesh>();
	return !mesh->IsEmpty();
}


Microsoft::WRL::ComPtr<ID3D12CommandList> MeshRenderer::Draw()
{
	if (!m_bInit) Initialize();
	Mesh* mesh = this->m_pGameObject->GetComponent<Mesh>();
	
	m_CommandAllocator->Reset();
	m_CommandList->Reset(m_CommandAllocator.Get(), nullptr);
	
	m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	m_CommandList->IASetIndexBuffer(&m_IndexBufferView);	
	
	//SetGraphicsRootSignature SetPipelineState
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = DX12Graphics::Instance->GetCurrentRtvDescriptor();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DX12Graphics::Instance->GetDsvDescriptor();

	D3D12_VIEWPORT viewPort = { 0,0,DX12Graphics::Instance->GetGraphicsWidth(),DX12Graphics::Instance->GetGraphicsHeight(),0,1 };
	m_CommandList->RSSetViewports(1, &viewPort);
	D3D12_RECT sissorRect = { 0,0,DX12Graphics::Instance->GetGraphicsWidth(),DX12Graphics::Instance->GetGraphicsHeight() };
	m_CommandList->RSSetScissorRects(1, &sissorRect);
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	
	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->SetGraphicsRootSignature(m_Material->GetRootSignature());
	m_CommandList->SetPipelineState(m_Material->GetPipelineStateObject());

	//绑定
	auto rsTable = m_Material->GetRootSignatureTable();
	for(int i = 0;i < rsTable.size();i++)
	{
		if(rsTable[i].ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV)			
		{
			m_CommandList->SetGraphicsRootConstantBufferView(rsTable[i].ParameterIndex, rsTable[i].BufferLocation);
		}
		else if(rsTable[i].ParameterType == D3D12_ROOT_PARAMETER_TYPE_SRV)
		{
			m_CommandList->SetGraphicsRootShaderResourceView(rsTable[i].ParameterIndex, rsTable[i].BufferLocation);
		}
		else if(rsTable[i].ParameterType == D3D12_ROOT_PARAMETER_TYPE_UAV)
		{
			m_CommandList->SetGraphicsRootUnorderedAccessView(rsTable[i].ParameterIndex, rsTable[i].BufferLocation);
		}
		else if(rsTable[i].ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			m_CommandList->SetDescriptorHeaps(1, &rsTable[i].pHeap);
			m_CommandList->SetGraphicsRootDescriptorTable(rsTable[i].ParameterIndex, rsTable[i].DescriptorHandle);
		}
	}
	
	m_CommandList->DrawIndexedInstanced(mesh->GetIndices().size(), 1, 0, 0, 0);
	m_CommandList->Close();
	
	return m_CommandList;
}

void MeshRenderer::SetMaterial(Material* material)
{
	m_Material = material;
}

