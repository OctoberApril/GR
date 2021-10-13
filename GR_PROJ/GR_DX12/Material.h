#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "HObject.h"
#include<unordered_map>
#include<string>
#include <glm/fwd.hpp>


static const InterfaceID IID_MATERIAL = { 4,4,0,{0,0,0,0,0,0,0,0} };

class HPass;
class Texture2D;
class UploadBuffer;

class Material : public HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_MATERIAL, HObject)
public:
	
	Material(const wchar_t* vsShader, const wchar_t* psShader);
	~Material() override;


	void SetMatrix(std::string variableName, glm::mat4 matrix);
	void SetTexture2D(std::string variableName, Texture2D* texture);

	ID3D12RootSignature* GetRootSignature() const;
	ID3D12PipelineState* GetPipelineStateObject() const;

	void Update(ComPtr<ID3D12GraphicsCommandList> commandlist) const;

protected:
	
	std::unordered_map<std::string, glm::mat4> m_MatrixVariableMap;
	std::unordered_map<std::string, SharedPtr<Texture2D>> m_Texture2DVariableMap;

	UploadBuffer* m_UploadBuffer;
	
	HPass* m_pHPass;
};

