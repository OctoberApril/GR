#pragma once
#include <string>
#include <wrl.h>
#include <unordered_set>

#include"d3dx12.h"
#include "HObject.h"
#include "ReflectionTable.h"


static const InterfaceID IID_HPASS = { 5,0,0,{0,0,0,0,0,0,0,0} };

enum ShaderType
{
	UnDefined,
	ShaderType_Float,
	ShaderType_Vector3,
	ShaderType_Vector4,
	ShaderType_Texture2D,
	ShaderType_Matrix,
};

namespace std
{
	template<>
	struct hash<ShaderType>
	{
		size_t operator()(const ShaderType type_value) const noexcept
		{
			return static_cast<std::size_t>(type_value);
		}
	};
}


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

	ComPtr<ID3D12PipelineState> GetGraphicsPSO();
	void SetRootSignature(ID3D12RootSignature* rootSignaturePtr);

private:
	std::wstring m_VSShaderPath;
	std::wstring m_PSShaderPath;

	D3D12_INPUT_LAYOUT_DESC m_DefaultInputLayout;

	D3D12_BLEND_DESC m_BlendStatus;
	D3D12_RASTERIZER_DESC m_RasterizerStatus;
	D3D12_DEPTH_STENCIL_DESC m_DepthStencilStatus;

	ComPtr<ID3D12RootSignature> m_RootSignature;

	std::unordered_set<ReflectionTable, ReflectionTableHash> m_PassReflectTable;
};

