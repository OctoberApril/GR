#include "Material.h"
#include "HPass.h"
#include "Texture2D.h"
#include <glm/detail/type_mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Graphics.h"
#include "UploadBuffer.h"
#include "d3dx12.h"
#include "Scene.h"
#include "ThirdCamera.h"

Material::Material(const wchar_t* vsShader, const wchar_t* psShader) : m_pHPass(std::move(new HPass(vsShader, psShader))), m_UploadBuffer(new UploadBuffer(1024 * 1024))
{

}

Material::~Material()
{
	m_MatrixVariableMap.clear();
	m_Texture2DVariableMap.clear();
	delete m_pHPass;
}

void Material::SetMatrix(std::string variableName, glm::mat4 matrix)
{	
	m_MatrixVariableMap.emplace(variableName, matrix);
}

void Material::SetTexture2D(std::string variableName, Texture2D* texture)
{
	m_Texture2DVariableMap.emplace(variableName, texture);
}

ID3D12PipelineState* Material::GetPipelineStateObject() const
{
	return m_pHPass->GetPipelineStateObject();
}

ID3D12RootSignature* Material::GetRootSignature() const
{
	return m_pHPass->GetRootSignature();
}

static Microsoft::WRL::ComPtr<ID3D12Resource> resource;

void Material::Update(ComPtr<ID3D12GraphicsCommandList> commandlist) const
{
	auto device = DX12Graphics::Instance->GetDevice();
	//for(auto p = m_MatrixVariableMap.begin();p != m_MatrixVariableMap.end();p++)
	//{
	//	
	//}
	auto camera = Scene::Default->GetSceneCamera();
	glm::mat4 viewMat = camera->GetViewMatrix();
	glm::mat4 projMat = camera->GetPerspectiveMatrix();
	
	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 0;
	heapProperties.VisibleNodeMask = 0;

	auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(glm::mat4) * 2);
	
	device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(resource.GetAddressOf()));
	byte* dst = nullptr;
	resource->Map(0, nullptr, (void**)&dst);
	auto viewBytes = (float*)glm::value_ptr(viewMat);
	auto projBytes = (float*)glm::value_ptr(projMat);
	memcpy(dst, viewBytes, sizeof(glm::mat4));
	memcpy(dst + sizeof(glm::mat4), projBytes, sizeof(glm::mat4));

	int paramIndex = m_pHPass->GetVariableIndexInRootSignature("GlobalCameraMatrix");
	commandlist->SetGraphicsRootConstantBufferView(paramIndex, resource->GetGPUVirtualAddress());
}





