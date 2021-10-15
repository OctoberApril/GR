#include "DefaultMaterial.h"
#include "Graphics.h"
#include "d3dx12.h"
#include "ShaderUniform.h"


DefaultMaterial::DefaultMaterial() :
	Material(L"./Shader/gpass_vs.hlsl", L"./Shader/gpass_ps.hlsl")
{
	BindResources();
}



void DefaultMaterial::BindResources()
{
	//绑定Sampler
	auto device = DX12Graphics::Instance->GetDevice();
	assert(device != nullptr);

	D3D12_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = 8;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	device->CreateSampler(&samplerDesc, m_ShaderUniformMap["defaultSampler"].CpuDescriptorHandle);

}
