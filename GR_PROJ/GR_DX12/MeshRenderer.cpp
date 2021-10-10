#include "MeshRenderer.h"

#include <iostream>

#include "GameObject.h"
#include "Material.h"
#include "Graphics.h"
#include "Mesh.h"
#include "UploadBuffer.h"

MeshRenderer::MeshRenderer() :m_Material(nullptr), m_CommandAllocator(nullptr), m_CommandList(nullptr), m_Device(nullptr), m_bInit(false),m_UploadBuffer(nullptr)
{
	m_Device = DX12Graphics::Instance->GetDevice();
	assert(m_Device != nullptr);
	m_UploadBuffer = new UploadBuffer();
	
	m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAllocator.GetAddressOf()));
	m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(m_CommandList.GetAddressOf()));
	m_CommandList->Close();
}


Microsoft::WRL::ComPtr<ID3D12CommandList> MeshRenderer::Draw()
{
	m_CommandAllocator->Reset();
	m_CommandList->Reset(m_CommandAllocator.Get(), nullptr);
	
	Mesh* mesh = this->m_pGameObject->GetComponent<Mesh>();
	assert(mesh != nullptr);
	auto vertices = mesh->GetVertices();
	auto indices = mesh->GetIndices();

	if(vertices.size() == 0 || indices.size() == 0)
	{
		m_CommandList->Close();
		return m_CommandList;
	}	

	//是否创建过VertexBuffer IndexBuffer 没有则创建
	if(!m_bInit)
	{
		BufferAllocation verticesBuffer = m_UploadBuffer->Allocate(vertices.size() * sizeof(glm::vec3), 4);
		BufferAllocation indicesBuffer = m_UploadBuffer->Allocate(indices.size() * sizeof(uint32_t), 4);

		memcpy(verticesBuffer.CpuAddress, &vertices[0], vertices.size() * sizeof(glm::vec3));
		m_VertexBufferView.BufferLocation = verticesBuffer.GpuAddress;
		m_VertexBufferView.SizeInBytes = vertices.size() * sizeof(glm::vec3);
		m_VertexBufferView.StrideInBytes = sizeof(glm::vec3);

		memcpy(indicesBuffer.CpuAddress, &indices[0], indices.size() * sizeof(uint32_t));
		m_IndexBufferView.BufferLocation = indicesBuffer.GpuAddress;
		m_IndexBufferView.SizeInBytes = indices.size() * sizeof(uint32_t);
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;

		m_bInit = true;
		std::cout << indices.size() / 3 << std::endl;
	}

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

	//m_CommandList->SetGraphicsRootDescriptorTable()
	
	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->SetGraphicsRootSignature(m_Material->GetRootSignature());
	m_CommandList->SetPipelineState(m_Material->GetPipelineStateObject());
	m_CommandList->DrawIndexedInstanced(indices.size(), 1, 0, 0, 0);
	m_CommandList->Close();
	
	return m_CommandList;
}

void MeshRenderer::SetMaterial(Material* material)
{
	m_Material = material;
}

