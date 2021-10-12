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
	template<typename T>  using ComPtr = Microsoft::WRL::ComPtr<T>;
	Material(const wchar_t* vsShader, const wchar_t* psShader);
	~Material() override;

	//RootSignature 最大长度 64dword = 64 * 4 = 256
	//Descriptor{D3DResource,D3D_VIEW_DESC} 收集该PASS中所有的View CopyAllDescriptor => DestDescriptorHeap
	//
	void SetMatrix(std::string variableName, glm::mat4 matrix);
	void SetTexture2D(std::string variableName, Texture2D* texture);

	ID3D12RootSignature* GetRootSignature() const;
	ID3D12PipelineState* GetPipelineStateObject() const;

	void AutoBindResourceViewToCommandlist(ID3D12GraphicsCommandList* cmd);

protected:
	
	std::unordered_map<std::string, glm::mat4> m_MatrixVariableMap;
	std::unordered_map<std::string, SharedPtr<Texture2D>> m_Texture2DVariableMap;

	UploadBuffer* m_UploadBuffer;
	
	HPass* m_pHPass;
};

