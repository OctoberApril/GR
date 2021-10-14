#pragma once
#include <d3d12.h>
#include <string>
#include <set>

static std::set<std::string> s_GlobalShaderUniform = { "HHQ_MATRIX_V","HHQ_MATRIX_P" };
static std::set<std::string> s_LocalShaderUniform = { "HHQ_MATRIX_Model"};

struct ShaderUniform
{	
	/// <summary>
	/// descriptor or descriptortable
	/// </summary>
	D3D12_ROOT_PARAMETER_TYPE UniformType;

	union 
	{
		//CpuDescriptor 用于创建资源 GpuDescritorHandle
		D3D12_CPU_DESCRIPTOR_HANDLE CpuDescriptorHandle;
		//
		D3D12_GPU_VIRTUAL_ADDRESS BufferLocation;
	};
};

struct RootSignatureTableBindItem
{
	unsigned int ParameterIndex;

	D3D12_ROOT_PARAMETER_TYPE ParameterType;
	
	union
	{		
		D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHandle;
		D3D12_GPU_VIRTUAL_ADDRESS BufferLocation;
	};
};