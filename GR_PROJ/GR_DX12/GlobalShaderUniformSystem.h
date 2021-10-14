#pragma once
#include <unordered_map>
#include <wrl.h>
#include <d3d12.h>

class ShaderUniform;

class GlobalShaderUniformSystem
{
public:
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	
	GlobalShaderUniformSystem();
	~GlobalShaderUniformSystem();

	void Initialize();
	void Update();
	
	bool GetShaderUniform(std::string uniform_name,ShaderUniform* ret) const;

	static GlobalShaderUniformSystem* Instance;

private:

	std::unordered_map<std::string, ComPtr<ID3D12Resource>> m_GlobalUniformBufferMap;
	
	std::unordered_map<std::string, ShaderUniform> m_GlobalUniformMap;
};

