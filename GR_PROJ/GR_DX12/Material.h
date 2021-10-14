#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "HObject.h"
#include<unordered_map>
#include<string>
#include <glm/fwd.hpp>


class HPass;
class Texture2D;
struct RootSignatureTableBindItem;
struct ShaderUniform;

static const InterfaceID IID_MATERIAL = { 4,3,0,{0,0,0,0,0,0,0,0} };

class Material : public HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_MATERIAL, HObject)
public:
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	
	Material(const wchar_t* vsShader, const wchar_t* psShader);
	virtual ~Material();


	void SetMatrix(std::string variableName, glm::mat4 matrix);
	void SetTexture2D(std::string variableName, Texture2D* texture);

	ID3D12RootSignature* GetRootSignature() const;
	ID3D12PipelineState* GetPipelineStateObject() const;
	
	virtual void BindResources();
	void InitRootSignatureResource();
	std::vector<RootSignatureTableBindItem> GetRootSignatureTable() const;
	int GetParameterIndex(std::string uniform_name) const;

protected:

	//用于Commandlist绑定资源
	std::vector<RootSignatureTableBindItem> m_RootSignatureBindItems;

	std::unordered_map<std::string, ShaderUniform> m_ShaderUniformMap;
	
	HPass* m_pHPass;
};

