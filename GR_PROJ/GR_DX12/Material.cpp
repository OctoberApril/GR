#include "Material.h"
#include "HPass.h"
#include "Texture2D.h"
#include <glm/detail/type_mat4x4.hpp>
#include "Graphics.h"
#include "UploadBuffer.h"
#include "DescriptorAllocation.h"


Material::Material(const wchar_t* vsShader, const wchar_t* psShader) : m_pHPass(std::move(new HPass(vsShader, psShader))), m_UploadBuffer(new UploadBuffer(1024 * 1024))
{

}

Material::~Material()
{
	m_MatrixVariableMap.clear();
	m_Texture2DVariableMap.clear();
	delete m_pHPass;
}

void Material::SetMatrix(std::string variableName, glm::mat4 matrix)
{
	if (m_pHPass != nullptr)
	{
		if (!m_pHPass->CheckVariableIsValidate(variableName,sizeof(matrix)))
		{
			char* buffer = new char[256];
			sprintf_s(buffer, 256, "Shader中不存在变量:%s或者类型不一致,验证失败.", variableName);
			OutputDebugStringA(buffer);
		}
	}
	
	//DX12Graphics::Instance->AllocateDescriptors(d3d12_);
	m_MatrixVariableMap.emplace(variableName, matrix);
}

void Material::SetTexture2D(std::string variableName, Texture2D* texture)
{
	if (m_pHPass != nullptr)
	{
		if (!m_pHPass->CheckVariableIsValidate(variableName))
		{
			char* buffer = new char[256];
			sprintf_s(buffer,256,"Shader中不存在变量:%s,验证失败.",variableName);
			OutputDebugStringA(buffer);
		}
	}
	m_Texture2DVariableMap.emplace(variableName, texture);
}

ID3D12PipelineState* Material::GetPipelineStateObject() const
{
	return m_pHPass->GetPipelineStateObject();
}

ID3D12RootSignature* Material::GetRootSignature() const
{
	return m_pHPass->GetRootSignature();
}

void Material::AutoBindResourceViewToCommandlist(ID3D12GraphicsCommandList* cmd)
{
	auto device = DX12Graphics::Instance->GetDevice();
	
	auto descriptorInfo = DX12Graphics::Instance->AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_MatrixVariableMap.size());	
	int index = 0;
	for(auto p = m_MatrixVariableMap.begin();p != m_MatrixVariableMap.end();p++, index++)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC constBuffer = {};

		auto cbBuffer = m_UploadBuffer->Allocate(sizeof(glm::mat4), 4);
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbDesc = { cbBuffer.GpuAddress,cbBuffer.Size };

		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = descriptorInfo.CPU;
		cpuHandle.ptr += index * descriptorInfo.IncrementInDescriptorHeap;		
		device->CreateConstantBufferView(&cbDesc, cpuHandle);
	}
}




