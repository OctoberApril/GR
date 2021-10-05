#pragma once
#include <d3dcommon.h>
#include <string>
#include <unordered_map>
#include <vector>

enum ShaderStage
{
	ShaderStage_Unkonw = 0,
	ShaderStage_VS = (ShaderStage_Unkonw + 1),
	ShaderStage_PS = (ShaderStage_VS + 1),
};

struct ResourceVariableInfo
{
	ResourceVariableInfo(std::string name, unsigned int bindPoint, unsigned int bindCount, unsigned int space)
		:Name(name), BindPoint(bindPoint), BindCount(bindCount), Space(space) {}
	
	std::string Name;
	unsigned int BindPoint;
	unsigned int BindCount;
	unsigned int Space;
};

#pragma Constant Buffer Info

struct SubConstantVariableInfo
{
	std::string SubName;
	unsigned int StartOffset;
	unsigned int Size;
};

struct ConstantVariableInfo : public ResourceVariableInfo
{
	ConstantVariableInfo(std::string name, unsigned int bindPoint, unsigned int bindCount,unsigned int space) :ResourceVariableInfo(name, bindPoint, bindCount, space)
	{

	}

	D3D_CBUFFER_TYPE Type;
	unsigned int Variables;
	unsigned int Size;

	std::vector<SubConstantVariableInfo> SubVariables;
};

#pragma endregion




struct ReflectionTable
{
	ShaderStage Stage;

	std::unordered_map<std::string, ConstantVariableInfo> ConstantMap;

	std::unordered_map<std::string, ResourceVariableInfo> SamplerMap;

	std::unordered_map<std::string, ResourceVariableInfo> Texture2DMap;

	bool operator ==(const ReflectionTable& other) const
	{
		return Stage == other.Stage;
	}
};

struct ReflectionTableHash
{
	std::size_t operator()(ReflectionTable t) const
	{
		return static_cast<std::size_t>(t.Stage);
	}
};


