#include <cassert>
#include <d3dcompiler.h>

#include "HPass.h"

#include <algorithm>

#include "Graphics.h"
#include "Helper.h"
#include "Win32App.h"

/// <summary>
/// 全局预定义 非DescriptorTable的变量
/// </summary>
std::unordered_map<std::string, DescriptorType> HPass::g_PreDefineAccess = { {"GlobalCameraMatrix", Descriptor} };

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
	RebuildPipelineStateObject();
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

	std::vector<ReflectionShaderVariableInfo> vsVariables, psVariables;

	D3D12_SHADER_DESC shaderDesc = {};

#pragma region 分析VSShader
	{
		vsReflection->GetDesc(&shaderDesc);
		for (int i = 0; i < shaderDesc.BoundResources; i++)
		{
			D3D12_SHADER_INPUT_BIND_DESC bindDesc = {};
			vsReflection->GetResourceBindingDesc(i, &bindDesc);

			if (bindDesc.Type == D3D_SIT_TEXTURE || bindDesc.Type == D3D_SIT_SAMPLER
				|| bindDesc.Type == D3D_SIT_CBUFFER || bindDesc.Type == D3D_SIT_TBUFFER)
			{
				ReflectionShaderVariableInfo info = {};
				info.VariableName = bindDesc.Name;
				info.Type = bindDesc.Type;
				info.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
				info.BindPoint = bindDesc.BindPoint;
				info.BindCount = bindDesc.BindCount;
				info.Space = bindDesc.Space;

				vsVariables.emplace_back(info);
			}
		}

		for (int i = 0; i < shaderDesc.ConstantBuffers; i++)
		{
			auto pConstantInfo = vsReflection->GetConstantBufferByIndex(i);
			D3D12_SHADER_BUFFER_DESC bufferDesc = {};
			pConstantInfo->GetDesc(&bufferDesc);

			auto iter = std::find_if(vsVariables.begin(), vsVariables.end(), [bufferDesc](const ReflectionShaderVariableInfo& info)
				{
					return info.VariableName == bufferDesc.Name;
				});
			assert(iter != vsVariables.end());

			iter->CBInfo.Name = bufferDesc.Name;
			iter->CBInfo.Type = bufferDesc.Type;
			iter->CBInfo.Size = bufferDesc.Size;
			iter->CBInfo.Variables = bufferDesc.Variables;

			for (int j = 0; j < bufferDesc.Variables; j++)
			{
				auto pReflectVariableInfo = pConstantInfo->GetVariableByIndex(j);
				D3D12_SHADER_VARIABLE_DESC variableDesc = {};
				pReflectVariableInfo->GetDesc(&variableDesc);
				iter->CBInfo.SubVariables.push_back(SubConstantVariableDesc{
					variableDesc.Name,variableDesc.StartOffset,variableDesc.Size,variableDesc.uFlags,
					variableDesc.DefaultValue,variableDesc.StartTexture,variableDesc.TextureSize,
					variableDesc.StartSampler,variableDesc.SamplerSize
					});
			}
		}
	}
#pragma endregion

#pragma region 分析PixelShader
	{
		psReflection->GetDesc(&shaderDesc);
		for (int i = 0; i < shaderDesc.BoundResources; i++)
		{
			D3D12_SHADER_INPUT_BIND_DESC bindDesc = {};
			psReflection->GetResourceBindingDesc(i, &bindDesc);

			if (bindDesc.Type == D3D_SIT_TEXTURE || bindDesc.Type == D3D_SIT_SAMPLER
				|| bindDesc.Type == D3D_SIT_CBUFFER || bindDesc.Type == D3D_SIT_TBUFFER)
			{
				ReflectionShaderVariableInfo info = {};
				info.VariableName = bindDesc.Name;
				info.Type = bindDesc.Type;
				info.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
				info.BindPoint = bindDesc.BindPoint;
				info.BindCount = bindDesc.BindCount;
				info.Space = bindDesc.Space;

				psVariables.emplace_back(info);
			}
		}

		for (int i = 0; i < shaderDesc.ConstantBuffers; i++)
		{
			auto pConstantInfo = psReflection->GetConstantBufferByIndex(i);
			D3D12_SHADER_BUFFER_DESC bufferDesc = {};
			pConstantInfo->GetDesc(&bufferDesc);

			auto iter = std::find_if(psVariables.begin(), psVariables.end(), [bufferDesc](const ReflectionShaderVariableInfo& info)
				{
					return info.VariableName == bufferDesc.Name;
				});
			assert(iter != psVariables.end());

			iter->CBInfo.Name = bufferDesc.Name;
			iter->CBInfo.Type = bufferDesc.Type;
			iter->CBInfo.Size = bufferDesc.Size;
			iter->CBInfo.Variables = bufferDesc.Variables;

			for (int j = 0; j < bufferDesc.Variables; j++)
			{
				auto pReflectVariableInfo = pConstantInfo->GetVariableByIndex(j);
				D3D12_SHADER_VARIABLE_DESC variableDesc = {};
				pReflectVariableInfo->GetDesc(&variableDesc);
				iter->CBInfo.SubVariables.push_back(SubConstantVariableDesc{
					variableDesc.Name,variableDesc.StartOffset,variableDesc.Size,variableDesc.uFlags,
					variableDesc.DefaultValue,variableDesc.StartTexture,variableDesc.TextureSize,
					variableDesc.StartSampler,variableDesc.SamplerSize
					});
			}
		}
	}
#pragma endregion

	m_PassReflectionInfo.insert(m_PassReflectionInfo.end(), vsVariables.begin(), vsVariables.end());

	for (auto p = psVariables.begin(); p != psVariables.end(); p++)
	{
		auto iter = std::find_if(m_PassReflectionInfo.begin(), m_PassReflectionInfo.end(), [p](const ReflectionShaderVariableInfo& lhs)
			{
				return *p == lhs;
			});

		if (iter != m_PassReflectionInfo.end())
			iter->ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		else
			m_PassReflectionInfo.emplace_back(*p);
	}
}

void HPass::RebuildRootSignature()
{
	auto device = DX12Graphics::Instance->GetDevice();
	assert(device != nullptr);


	std::vector<ReflectionShaderVariableInfo> passVariables = m_PassReflectionInfo;
	std::vector<RootSignatureParameter> rootParameters;

	for (auto p = g_PreDefineAccess.begin(); p != g_PreDefineAccess.end(); p++)
	{
		auto iter = std::find_if(passVariables.begin(), passVariables.end(), [p](const ReflectionShaderVariableInfo& lhs)
			{
				return lhs.VariableName == p->first;
			});

		if (iter != passVariables.end())
		{
			RootSignatureParameter parameter = {};
			parameter.VariableName = iter->VariableName;
			parameter.ParameterIndex = rootParameters.size();
			parameter.ShaderVisibility = iter->ShaderVisibility;
			
			if (p->second == Descriptor)
			{
				if (iter->Type == D3D_SIT_TEXTURE)
					parameter.RootParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
				else if (iter->Type == D3D_SIT_CBUFFER || iter->Type == D3D_SIT_TBUFFER)
					parameter.RootParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
				else
					parameter.RootParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			}
			else if (p->second == Constants)
			{
				parameter.RootParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			}
			else
				parameter.RootParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

			if (parameter.RootParameterType == D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS)
			{
				parameter.Constants.ShaderRegister = iter->BindPoint;
				parameter.Constants.RegisterSpace = iter->Space;
			}
			else if (parameter.RootParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV
				|| parameter.RootParameterType == D3D12_ROOT_PARAMETER_TYPE_SRV)
			{
				parameter.Descriptor.ShaderRegister = iter->BindPoint;
				parameter.Descriptor.RegisterSpace = iter->Space;
			}

			rootParameters.emplace_back(parameter);
			passVariables.erase(iter);
		}
	}

	//对剩余的进行排序 ShaderVisibility->CBV|Texture2D|Sampler
	std::sort(passVariables.begin(), passVariables.end(), [](const ReflectionShaderVariableInfo& lhs, const ReflectionShaderVariableInfo& rhs)->bool
		{
			return (lhs.ShaderVisibility < rhs.ShaderVisibility) || (lhs.Type < rhs.Type);
		});

	for (auto p = passVariables.begin(); p != passVariables.end(); p++)
	{
		RootSignatureParameter parameter = {};
		parameter.VariableName = p->VariableName;
		parameter.RootParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		parameter.ShaderVisibility = p->ShaderVisibility;
		parameter.DescriptorTable.Size = p->BindCount;
		parameter.DescriptorTable.ShaderRegister = p->BindPoint;
		parameter.DescriptorTable.RegisterSpace = p->Space;

		if (p->Type == D3D_SIT_TEXTURE)
		{
			parameter.DescriptorTable.HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			parameter.DescriptorTable.DescriptorIncremental = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			parameter.DescriptorTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		}
		else if(p->Type == D3D_SIT_CBUFFER || p->Type == D3D_SIT_TBUFFER)
		{
			parameter.DescriptorTable.HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			parameter.DescriptorTable.DescriptorIncremental = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			parameter.DescriptorTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		}
		else if (p->Type == D3D_SIT_SAMPLER)
		{
			parameter.DescriptorTable.HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			parameter.DescriptorTable.DescriptorIncremental = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
			parameter.DescriptorTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		}

		//和上一个存放的root比较是否可以合并
		if (rootParameters.size() <= 0)
		{
			parameter.ParameterIndex = rootParameters.size();
			parameter.DescriptorTable.Offset = 0;
		}
		else
		{
			if (rootParameters[rootParameters.size() - 1].RootParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE
				|| rootParameters[rootParameters.size() - 1].DescriptorTable.HeapType == parameter.DescriptorTable.HeapType)
			{
				parameter.ParameterIndex = rootParameters[rootParameters.size() - 1].ParameterIndex;
				parameter.DescriptorTable.Offset = rootParameters[rootParameters.size() - 1].DescriptorTable.Offset + rootParameters[rootParameters.size() - 1].DescriptorTable.Size;
			}
			else
			{
				parameter.ParameterIndex = rootParameters.size();
				parameter.DescriptorTable.Offset = 0;
			}
		}

		rootParameters.emplace_back(parameter);
	}

	
	//根据parameterIndex 归纳信息
	std::unordered_map<int, std::vector<RootSignatureParameter>> rootSignatureMap;
	for(auto p = rootParameters.begin();p != rootParameters.end();p++)
	{		
		rootSignatureMap[p->ParameterIndex].emplace_back(*p);
		m_RootParameterMap.emplace(p->VariableName, *p);
	}
	
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	CD3DX12_ROOT_PARAMETER* pParams = new CD3DX12_ROOT_PARAMETER[rootSignatureMap.size()];
	rootSignatureDesc.Init(rootSignatureMap.size(), pParams, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	for(auto p = rootSignatureMap.begin();p != rootSignatureMap.end();p++)
	{
		auto parameter = p->second[0];
		if(parameter.RootParameterType == D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS)
		{
			pParams[parameter.ParameterIndex].InitAsConstants(parameter.Constants.Size,
				parameter.Constants.ShaderRegister,
				parameter.Constants.RegisterSpace,
				parameter.ShaderVisibility);
		}
		else if (parameter.RootParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV)
		{
			pParams[parameter.ParameterIndex].InitAsConstantBufferView(parameter.Descriptor.ShaderRegister,
				parameter.Descriptor.RegisterSpace,
				parameter.ShaderVisibility);
		}
		else if (parameter.RootParameterType == D3D12_ROOT_PARAMETER_TYPE_SRV)
		{
			pParams[parameter.ParameterIndex].InitAsShaderResourceView(parameter.Descriptor.ShaderRegister,
				parameter.Descriptor.RegisterSpace,
				parameter.ShaderVisibility);
		}
		else if (parameter.RootParameterType == D3D12_ROOT_PARAMETER_TYPE_UAV)
		{
			pParams[parameter.ParameterIndex].InitAsUnorderedAccessView(parameter.Descriptor.ShaderRegister,
				parameter.Descriptor.RegisterSpace,
				parameter.ShaderVisibility);
		}
		else 
		{
			//处理DescriptorTable情况
			D3D12_DESCRIPTOR_RANGE* pRange = new D3D12_DESCRIPTOR_RANGE[p->second.size()];

			for(int j = 0;j < p->second.size();j++)
			{
				pRange[j].BaseShaderRegister = p->second[j].DescriptorTable.ShaderRegister;
				pRange[j].RegisterSpace = p->second[j].DescriptorTable.RegisterSpace;
				pRange[j].NumDescriptors = p->second[j].DescriptorTable.Size;
				pRange[j].OffsetInDescriptorsFromTableStart = p->second[j].DescriptorTable.Offset;
				pRange[j].RangeType = p->second[j].DescriptorTable.RangeType;
			}
			
			pParams[parameter.ParameterIndex].InitAsDescriptorTable(p->second.size(), pRange, parameter.ShaderVisibility);
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

	D3D12_INPUT_ELEMENT_DESC elements[1];
	psoDesc.InputLayout.NumElements = 1;
	psoDesc.InputLayout.pInputElementDescs = elements;
	elements[0].AlignedByteOffset = 0;
	elements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	elements[0].InputSlot = 0;
	elements[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	elements[0].SemanticIndex = 0;
	elements[0].SemanticName = "POSITION";
	elements[0].InstanceDataStepRate = 0;

	/*elements[1].AlignedByteOffset = sizeof(float) * 3;
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
	elements[3].InstanceDataStepRate = 0;*/

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

ID3D12PipelineState* HPass::GetPipelineStateObject() const
{
	return m_PipelineState.Get();
}

ID3D12RootSignature* HPass::GetRootSignature() const
{
	return m_RootSignature.Get();
}

int HPass::GetVariableIndexInRootSignature(std::string variable_name) const
{
	auto p = m_RootParameterMap.find(variable_name);
	if (p != m_RootParameterMap.end())
		return p->second.ParameterIndex;

	return -1;
}








