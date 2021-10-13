#pragma once
#include <string>
#include <wrl.h>
#include <unordered_map>
#include"d3dx12.h"
#include "HObject.h"
#include "ShaderReflection.h"

static const InterfaceID IID_HPASS = { 5,0,0,{0,0,0,0,0,0,0,0} };


class HPass : HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_HPASS, HObject)
public:
	HPass(
		const wchar_t* vsShaderPath,
		const wchar_t* psShaderPath,
		CD3DX12_BLEND_DESC blend = CD3DX12_BLEND_DESC(D3D12_DEFAULT),
		CD3DX12_RASTERIZER_DESC rasterizer = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
		CD3DX12_DEPTH_STENCIL_DESC depth_stencil = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
	);
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	void SetVertexShader(const std::wstring& vsPath);
	std::wstring GetVertexShaderPath() const;

	void SetPixelShader(const std::wstring& psPath);
	std::wstring GetPixelShaderPath() const;

	void SetBlendStatus(CD3DX12_BLEND_DESC desc);
	D3D12_BLEND_DESC GetBlendStatus() const;

	void SetRasterizerStatus(CD3DX12_RASTERIZER_DESC rasterizerDesc);
	D3D12_RASTERIZER_DESC GetRasterizerStatus() const;

	void SetDepthStencilStatus(CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc);
	D3D12_DEPTH_STENCIL_DESC GetDepthStencilStatus() const;

	ID3D12PipelineState* GetPipelineStateObject() const;
	ID3D12RootSignature* GetRootSignature() const;
	int GetVariableIndexInRootSignature(std::string variable_name) const;

private:
	void RebuildShaderBlob();
	void RebuildPassReflectionTable();
	void RebuildRootSignature();
	void RebuildPipelineStateObject();

private:
	std::wstring m_VSShaderPath;
	std::wstring m_PSShaderPath;

	ComPtr<ID3DBlob> m_VSBlob;
	ComPtr<ID3DBlob> m_PSBlob;

	
	D3D12_BLEND_DESC m_BlendStatus;
	D3D12_RASTERIZER_DESC m_RasterizerStatus;
	D3D12_DEPTH_STENCIL_DESC m_DepthStencilStatus;

	ComPtr<ID3D12RootSignature> m_RootSignature;
	ComPtr<ID3D12PipelineState> m_PipelineState;
	
	std::vector<ReflectionShaderVariableInfo> m_PassReflectionInfo;

	std::unordered_map<std::string, RootSignatureParameter> m_RootParameterMap;
	
	static std::unordered_map<std::string, DescriptorType> g_PreDefineAccess;
};

