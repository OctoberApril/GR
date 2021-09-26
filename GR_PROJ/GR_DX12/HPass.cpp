#include <cassert>
#include <d3dcompiler.h>

#include "HPass.h"
#include "Helper.h"


void HPass::SetVertexShader(const std::wstring& vsPath)
{	
	m_VSShaderPath = vsPath;
}

std::wstring HPass::GetVertexShaderPath() const
{
	return m_VSShaderPath;
}


void HPass::SetPixelShader(const std::wstring& psPath)
{
	m_PSShaderPath = psPath;
}

std::wstring HPass::GetPixelShaderPath() const
{
	return m_PSShaderPath;
}

void HPass::SetBlendStatus(CD3DX12_BLEND_DESC desc)
{
	m_BlendStatus = desc;
}

D3D12_BLEND_DESC HPass::GetBlendStatus() const
{
	return m_BlendStatus;
}

void HPass::SetRasterizerStatus(CD3DX12_RASTERIZER_DESC rasterizerDesc)
{
	m_RasterizerStatus = rasterizerDesc;
}

D3D12_RASTERIZER_DESC HPass::GetRasterizerStatus() const
{
	return m_RasterizerStatus;
}

void HPass::SetDepthStencilStatus(CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc)
{
	m_DepthStencilStatus = depthStencilDesc;
}


D3D12_DEPTH_STENCIL_DESC HPass::GetDepthStencilStatus() const
{
	return m_DepthStencilStatus;
}

void HPass::SetRootSignature(ID3D12RootSignature* rootSignaturePtr)
{
	m_RootSignature = rootSignaturePtr;
}


Microsoft::WRL::ComPtr<ID3D12PipelineState> HPass::GetGraphicsPSO(ID3D12Device* device) const
{
	assert(m_RootSignature != nullptr);
	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	
	ComPtr<ID3DBlob> errorMsg;
	ComPtr<ID3DBlob> vsShaderBlob;
	ComPtr<ID3DBlob> psShaderBlob;
	
	HRESULT hr = D3DCompileFromFile(m_VSShaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, vsShaderBlob.GetAddressOf(), errorMsg.GetAddressOf());
	if (errorMsg != nullptr)
	{
		OutputDebugStringA(static_cast<LPCSTR>(errorMsg->GetBufferPointer()));
	}
	ThrowIfFailed(hr);

	hr = D3DCompileFromFile(m_PSShaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, psShaderBlob.GetAddressOf(), errorMsg.GetAddressOf());
	if (errorMsg != nullptr)
	{
		OutputDebugStringA(static_cast<LPCSTR>(errorMsg->GetBufferPointer()));
	}
	ThrowIfFailed(hr);

	psoDesc.VS = { vsShaderBlob->GetBufferPointer(), vsShaderBlob->GetBufferSize() };
	psoDesc.PS = { psShaderBlob->GetBufferPointer(), psShaderBlob->GetBufferSize() };

	psoDesc.BlendState = m_BlendStatus;
	psoDesc.RasterizerState = m_RasterizerStatus;
	psoDesc.DepthStencilState = m_DepthStencilStatus;
	
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	psoDesc.NodeMask = 0;

	D3D12_INPUT_ELEMENT_DESC elements[4];
	psoDesc.InputLayout.NumElements = 4;
	psoDesc.InputLayout.pInputElementDescs = elements;
	elements[0].AlignedByteOffset = 0;
	elements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	elements[0].InputSlot = 0;
	elements[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	elements[0].SemanticIndex = 0;
	elements[0].SemanticName = "POSITION";
	elements[0].InstanceDataStepRate = 0;
	
	elements[1].AlignedByteOffset = sizeof(float) * 3;
	elements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	elements[1].InputSlot = 0;
	elements[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	elements[1].SemanticIndex = 0;
	elements[1].SemanticName = "COLOR";
	elements[1].InstanceDataStepRate = 0;
	
	elements[2].AlignedByteOffset = sizeof(float) * 7;
	elements[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	elements[2].InputSlot = 0;
	elements[2].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	elements[2].SemanticIndex = 0;
	elements[2].SemanticName = "NORMAL";
	elements[2].InstanceDataStepRate = 0;

	elements[3].AlignedByteOffset = sizeof(float) * 10;
	elements[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	elements[3].InputSlot = 0;
	elements[3].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	elements[3].SemanticIndex = 0;
	elements[3].SemanticName = "TEXCOORD";
	elements[3].InstanceDataStepRate = 0;

	//反射Shader拿到 rootsignature的信息 用于设置
	//material 给hpass设置 rootsignature
	
	psoDesc.pRootSignature = m_RootSignature.Get();

	ComPtr<ID3D12PipelineState> graphicsPipelineState;
	ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(graphicsPipelineState.GetAddressOf())));

	return graphicsPipelineState;
}









