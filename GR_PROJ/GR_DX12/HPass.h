#pragma once
#include <string>
#include <wrl.h>

#include"d3dx12.h"
#include "HObject.h"


static const InterfaceID IID_HPASS = { 5,0,0,{0,0,0,0,0,0,0,0} };

class HPass : HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_HPASS, HObject)
public:
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;	
	HPass() = default;


	void SetVertexShader(const std::wstring& vsPath);
	std::wstring GetVertexShaderPath() const;

	void SetPixelShader(const std::wstring& psPath);
	std::wstring GetPixelShaderPath() const;

	void SetBlendStatus(CD3DX12_BLEND_DESC desc = CD3DX12_BLEND_DESC(D3D12_DEFAULT));
	D3D12_BLEND_DESC GetBlendStatus() const;

	void SetRasterizerStatus(CD3DX12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT));
	D3D12_RASTERIZER_DESC GetRasterizerStatus() const;

	void SetDepthStencilStatus(CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT));
	D3D12_DEPTH_STENCIL_DESC GetDepthStencilStatus() const;

	ComPtr<ID3D12PipelineState> GetGraphicsPSO(ID3D12Device* device) const;
	void SetRootSignature(ID3D12RootSignature* rootSignaturePtr);

private:
	std::wstring m_VSShaderPath;
	std::wstring m_PSShaderPath;

	D3D12_INPUT_LAYOUT_DESC m_DefaultInputLayout;

	D3D12_BLEND_DESC m_BlendStatus;
	D3D12_RASTERIZER_DESC m_RasterizerStatus;
	D3D12_DEPTH_STENCIL_DESC m_DepthStencilStatus;

	ComPtr<ID3D12RootSignature> m_RootSignature;
};

