#pragma once
#include <string>
#include <unordered_map>

enum ShaderStage
{
	ShaderStage_Unkonw = 0,
	ShaderStage_VS = (ShaderStage_Unkonw + 1),
	ShaderStage_PS = (ShaderStage_VS + 1),
};

struct ShaderVariableInfo
{
	std::string Name;
	unsigned int StartOffset;
	unsigned int Size;
};

struct ResourceVariableInfo
{
	std::string Name;
	unsigned int BindPoint;
	unsigned int BindCount;
};

struct ReflectionTable
{
	ShaderStage Stage;

	std::unordered_map<std::string, ShaderVariableInfo> ConstantMap;

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


