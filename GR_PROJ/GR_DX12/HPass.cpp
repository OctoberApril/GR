#include <cassert>
#include <d3dcompiler.h>

#include "HPass.h"

#include "Graphics.h"
#include "Helper.h"
#include "Win32App.h"


HPass::HPass(
	const wchar_t* vsShaderPath,
	const wchar_t* psShaderPath,
	CD3DX12_BLEND_DESC blend,
	CD3DX12_RASTERIZER_DESC rasterizer,
	CD3DX12_DEPTH_STENCIL_DESC depth_stencil
)
{
	assert(vsShaderPath != nullptr && psShaderPath != nullptr);
	m_VSShaderPath = vsShaderPath;
	m_PSShaderPath = psShaderPath;

	m_BlendStatus = blend;
	m_RasterizerStatus = rasterizer;
	m_DepthStencilStatus = depth_stencil;

	RebuildShaderBlob();
	RebuildPassReflectionTable();
	RebuildRootSignature();
}

void HPass::RebuildShaderBlob()
{
	unsigned int compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ALL_RESOURCES_BOUND;
	ComPtr<ID3DBlob> errorBlob;
	ThrowIfFailed(D3DCompileFromFile(m_VSShaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_1", compileFlags, 0, m_VSBlob.GetAddressOf(), errorBlob.GetAddressOf()));
	if (errorBlob != nullptr)
	{
		OutputDebugStringA((LPCSTR)errorBlob->GetBufferPointer());
		return;
	}

	ThrowIfFailed(D3DCompileFromFile(m_PSShaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_1", compileFlags, 0, m_PSBlob.GetAddressOf(), errorBlob.GetAddressOf()));
	if (errorBlob != nullptr)
	{
		OutputDebugStringA((LPCSTR)errorBlob->GetBufferPointer());
	}
}

void HPass::RebuildPassReflectionTable()
{
	assert(m_VSBlob != nullptr && m_PSBlob != nullptr);
	ComPtr<ID3D12ShaderReflection> vsReflection, psReflection;
	ThrowIfFailed(D3DReflect(m_VSBlob->GetBufferPointer(), m_VSBlob->GetBufferSize(), IID_ID3D12ShaderReflection, reinterpret_cast<void**>(vsReflection.GetAddressOf())));
	ThrowIfFailed(D3DReflect(m_PSBlob->GetBufferPointer(), m_PSBlob->GetBufferSize(), IID_ID3D12ShaderReflection, reinterpret_cast<void**>(psReflection.GetAddressOf())));

	ReflectionTable vsTable = { ShaderStage_VS }, psTable = { ShaderStage_PS };

	D3D12_SHADER_DESC shaderDesc = {};
	vsReflection->GetDesc(&shaderDesc);
	for (int i = 0; i < shaderDesc.BoundResources; i++)
	{
		D3D12_SHADER_INPUT_BIND_DESC bindDesc = {};
		vsReflection->GetResourceBindingDesc(i, &bindDesc);

		if (bindDesc.Type == D3D_SIT_TEXTURE)
		{
			vsTable.Texture2DMap.emplace(bindDesc.Name,ResourceVariableInfo{ bindDesc.Name,bindDesc.BindPoint,bindDesc.BindCount,bindDesc.Space });
		}
		else if (bindDesc.Type == D3D_SIT_SAMPLER)
		{
			vsTable.SamplerMap.emplace(bindDesc.Name,ResourceVariableInfo{ bindDesc.Name,bindDesc.BindPoint,bindDesc.BindCount,bindDesc.Space });
		}
		else if (bindDesc.Type == D3D_SIT_CBUFFER || bindDesc.Type == D3D_SIT_TBUFFER)
		{
			vsTable.ConstantMap.emplace(bindDesc.Name,ConstantVariableInfo{ bindDesc.Name,bindDesc.BindPoint,bindDesc.BindCount,bindDesc.Space });
		}
	}

	for (int i = 0; i < shaderDesc.ConstantBuffers; i++)
	{
		auto pConstantInfo = vsReflection->GetConstantBufferByIndex(i);
		D3D12_SHADER_BUFFER_DESC bufferDesc = {};
		pConstantInfo->GetDesc(&bufferDesc);

		ConstantVariableInfo variableInfo = vsTable.ConstantMap.find(bufferDesc.Name)->second;
		variableInfo.Type = bufferDesc.Type;
		variableInfo.Size = bufferDesc.Size;
		variableInfo.Variables = bufferDesc.Variables;

		for (int j = 0; j < bufferDesc.Variables; j++)
		{
			auto pReflectVariableInfo = pConstantInfo->GetVariableByIndex(j);
			D3D12_SHADER_VARIABLE_DESC variableDesc = {};
			pReflectVariableInfo->GetDesc(&variableDesc);
			variableInfo.SubVariables.push_back(SubConstantVariableInfo{ variableDesc.Name,variableDesc.StartOffset,variableDesc.Size });
		}
	}

	psReflection->GetDesc(&shaderDesc);
	for (int i = 0; i < shaderDesc.BoundResources; i++)
	{
		D3D12_SHADER_INPUT_BIND_DESC bindDesc = {};
		psReflection->GetResourceBindingDesc(i, &bindDesc);

		if (bindDesc.Type == D3D_SIT_TEXTURE)
		{
			psTable.Texture2DMap.emplace(bindDesc.Name,ResourceVariableInfo{ bindDesc.Name,bindDesc.BindPoint,bindDesc.BindCount,bindDesc.Space });
		}
		else if (bindDesc.Type == D3D_SIT_SAMPLER)
		{
			psTable.SamplerMap.emplace(bindDesc.Name,ResourceVariableInfo{ bindDesc.Name,bindDesc.BindPoint,bindDesc.BindCount ,bindDesc.Space });
		}
		else if (bindDesc.Type == D3D_SIT_CBUFFER || bindDesc.Type == D3D_SIT_TBUFFER)
		{
			psTable.ConstantMap.emplace(bindDesc.Name,ConstantVariableInfo{ bindDesc.Name,bindDesc.BindPoint,bindDesc.BindCount ,bindDesc.Space });
		}
	}

	for (int i = 0; i < shaderDesc.ConstantBuffers; i++)
	{
		auto pConstantInfo = psReflection->GetConstantBufferByIndex(i);
		D3D12_SHADER_BUFFER_DESC bufferDesc = {};
		pConstantInfo->GetDesc(&bufferDesc);

		ConstantVariableInfo variableInfo = psTable.ConstantMap.find(bufferDesc.Name)->second;
		variableInfo.Type = bufferDesc.Type;
		variableInfo.Size = bufferDesc.Size;
		variableInfo.Variables = bufferDesc.Variables;

		for (int j = 0; j < bufferDesc.Variables; j++)
		{
			auto pReflectVariableInfo = pConstantInfo->GetVariableByIndex(j);
			D3D12_SHADER_VARIABLE_DESC variableDesc = {};
			pReflectVariableInfo->GetDesc(&variableDesc);
			variableInfo.SubVariables.push_back(SubConstantVariableInfo{ variableDesc.Name,variableDesc.StartOffset,variableDesc.Size });
		}
	}

	m_PassReflectTable.emplace(vsTable);
	m_PassReflectTable.emplace(psTable);
}

void HPass::RebuildRootSignature()
{
	auto device = DX12Graphics::Instance->GetDevice();
	assert(device != nullptr);

	
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	int params = 0;
	for (auto p = m_PassReflectTable.begin(); p != m_PassReflectTable.end(); p++)
	{
		params += (*p).ConstantMap.size() + (*p).Texture2DMap.size() + (*p).SamplerMap.size();
	}
	CD3DX12_ROOT_PARAMETER* pParams = new CD3DX12_ROOT_PARAMETER[params];
	rootSignatureDesc.Init(params, pParams, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	int index = 0;
	for (auto p = m_PassReflectTable.begin(); p != m_PassReflectTable.end(); p++)
	{
		ReflectionTable tbInfo = *p;
		for (auto cb = tbInfo.ConstantMap.begin(); cb != tbInfo.ConstantMap.end(); cb++)
		{
			D3D12_DESCRIPTOR_RANGE* pRange = new D3D12_DESCRIPTOR_RANGE();
			pRange->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			pRange->NumDescriptors = 1;
			pRange->BaseShaderRegister = cb->second.BindPoint;
			pRange->RegisterSpace = cb->second.Space;			
			pRange->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			pParams[index++].InitAsDescriptorTable(1, pRange, tbInfo.Stage == ShaderStage_VS ? D3D12_SHADER_VISIBILITY_VERTEX : D3D12_SHADER_VISIBILITY_PIXEL);
		}

		for (auto cb = tbInfo.Texture2DMap.begin(); cb != tbInfo.Texture2DMap.end(); cb++)
		{
			D3D12_DESCRIPTOR_RANGE* pRange = new D3D12_DESCRIPTOR_RANGE();
			pRange->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			pRange->NumDescriptors = 1;
			pRange->BaseShaderRegister = cb->second.BindPoint;
			pRange->RegisterSpace = cb->second.Space;
			pRange->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			pParams[index++].InitAsDescriptorTable(1, pRange, tbInfo.Stage == ShaderStage_VS ? D3D12_SHADER_VISIBILITY_VERTEX : D3D12_SHADER_VISIBILITY_PIXEL);
		}

		for (auto cb = tbInfo.SamplerMap.begin(); cb != tbInfo.SamplerMap.end(); cb++)
		{
			D3D12_DESCRIPTOR_RANGE* pRange = new D3D12_DESCRIPTOR_RANGE();
			pRange->RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
			pRange->NumDescriptors = 1;
			pRange->BaseShaderRegister = cb->second.BindPoint;
			pRange->RegisterSpace = cb->second.Space;
			pRange->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			pParams[index++].InitAsDescriptorTable(1, pRange, tbInfo.Stage == ShaderStage_VS ? D3D12_SHADER_VISIBILITY_VERTEX : D3D12_SHADER_VISIBILITY_PIXEL);
		}
	}
		
	ComPtr<ID3DBlob> rootSignatureBlob, rootSignatureErrorBlob;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, rootSignatureBlob.GetAddressOf(), rootSignatureErrorBlob.GetAddressOf()));
	ThrowIfFailed(device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf())));	
}

void HPass::RebuildPipelineStateObject()
{
	auto device = DX12Graphics::Instance->GetDevice();
	assert(device != nullptr);
	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

	psoDesc.VS = { m_VSBlob->GetBufferPointer(), m_VSBlob->GetBufferSize() };
	psoDesc.PS = { m_PSBlob->GetBufferPointer(), m_PSBlob->GetBufferSize() };

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

	if (m_RootSignature == nullptr) RebuildRootSignature();
	psoDesc.pRootSignature = m_RootSignature.Get();

	ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_PipelineState.GetAddressOf())));
}


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

void HPass::SetBlendStatus(const CD3DX12_BLEND_DESC desc)
{
	m_BlendStatus = desc;
}

D3D12_BLEND_DESC HPass::GetBlendStatus() const
{
	return m_BlendStatus;
}

void HPass::SetRasterizerStatus(const CD3DX12_RASTERIZER_DESC rasterizerDesc)
{
	m_RasterizerStatus = rasterizerDesc;
}

D3D12_RASTERIZER_DESC HPass::GetRasterizerStatus() const
{
	return m_RasterizerStatus;
}

void HPass::SetDepthStencilStatus(const CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc)
{
	m_DepthStencilStatus = depthStencilDesc;
}


D3D12_DEPTH_STENCIL_DESC HPass::GetDepthStencilStatus() const
{
	return m_DepthStencilStatus;
}

ID3D12PipelineState* HPass::GetGraphicsPSO() const
{
	return m_PipelineState.Get();
}









