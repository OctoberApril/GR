#pragma once
#include <d3d12.h>
#include <string>
#include <vector>


/// <summary>
/// Ref D3D12_SHADER_VARIABLE_DESC
/// </summary>
struct SubConstantVariableDesc
{
	LPCSTR                  Name;           // Name of the variable
	UINT                    StartOffset;    // Offset in constant buffer's backing store
	UINT                    Size;           // Size of variable (in bytes)
	UINT                    uFlags;         // Variable flags
	LPVOID                  DefaultValue;   // Raw pointer to default value
	UINT                    StartTexture;   // First texture index (or -1 if no textures used)
	UINT                    TextureSize;    // Number of texture slots possibly used.
	UINT                    StartSampler;   // First sampler index (or -1 if no textures used)
	UINT                    SamplerSize;    // Number of sampler slots possibly used.

	bool operator ==(const SubConstantVariableDesc& other) const
	{
		return this->Name == other.Name
			&& this->StartOffset == other.StartOffset
			&& this->Size == other.Size
			&& this->uFlags == other.uFlags
			&& this->DefaultValue == other.DefaultValue
			&& this->StartTexture == other.StartTexture
			&& this->TextureSize == other.TextureSize
			&& this->StartSampler == other.StartSampler
			&& this->SamplerSize == other.SamplerSize;
	}
};

/// <summary>
/// D3D12_SHADER_BUFFER_DESC
/// </summary>
struct ConstantBufferDesc
{
	std::string             Name;           // Name of the constant buffer
	D3D_CBUFFER_TYPE        Type;           // Indicates type of buffer content
	UINT                    Variables;      // Number of member variables
	UINT                    Size;           // Size of CB (in bytes)
	UINT                    uFlags;         // Buffer description flags

	std::vector<SubConstantVariableDesc> SubVariables; //存放子变量的偏移

	bool operator ==(const ConstantBufferDesc& other) const
	{
		return this->Name == other.Name
			&& this->Type == other.Type
			&& this->Variables == other.Variables
			&& this->Size == other.Size
			&& this->uFlags == other.uFlags
			&& this->SubVariables == other.SubVariables;
	}
};

/// <summary>
/// Ref D3D12_SHADER_INPUT_BIND_DESC
/// </summary>
struct ReflectionShaderVariableInfo
{
	std::string VariableName;

	/// <summary>
	/// 暂时只分析 Texture2D CBuffer TBuffer Sampler
	/// </summary>
	D3D_SHADER_INPUT_TYPE Type;

	/// <summary>
	/// 可见性
	/// </summary>
	D3D12_SHADER_VISIBILITY ShaderVisibility;

	unsigned int BindPoint;
	unsigned int BindCount;
	unsigned int Space;

	ConstantBufferDesc CBInfo;

	bool operator ==(const ReflectionShaderVariableInfo& other) const
	{
		return this->VariableName == other.VariableName
			&& this->Type == other.Type
			&& this->BindPoint == other.BindPoint
			&& this->BindCount == other.BindCount
			&& this->Space == other.Space
			&& this->CBInfo == other.CBInfo;
	}
};


enum DescriptorType
{
	Constants = 0,
	Descriptor = (Constants + 1),
	DescriptorTable = (Descriptor + 1),
};

struct ConstantsParameter
{
	unsigned int ShaderRegister;

	unsigned int RegisterSpace;

	size_t Size;
};

struct DescriptorParameter
{	
	unsigned int ShaderRegister;
	
	unsigned int RegisterSpace;

	D3D12_GPU_VIRTUAL_ADDRESS BufferLocation;
};

struct DescriptorTableParameter
{
	unsigned int ShaderRegister;

	unsigned int RegisterSpace;
	
	D3D12_GPU_VIRTUAL_ADDRESS BaseDescriptor;

	D3D12_DESCRIPTOR_HEAP_TYPE HeapType;
	D3D12_DESCRIPTOR_RANGE_TYPE RangeType;

	size_t Offset; //在Descriptor中的偏移

	size_t Size;  //Descriptor的数量

	size_t DescriptorIncremental;
};

struct RootSignatureParameter
{
	std::string VariableName;

	uint32_t ParameterIndex;

	D3D12_ROOT_PARAMETER_TYPE RootParameterType;

	D3D12_SHADER_VISIBILITY ShaderVisibility;
	
	union
	{
		ConstantsParameter Constants;
		DescriptorParameter Descriptor;
		DescriptorTableParameter DescriptorTable;
	};
};

struct RootSignatureParameterHash
{
	size_t operator()(const RootSignatureParameter& t) const
	{		
		return std::hash<std::string>()(t.VariableName);
	}
};

