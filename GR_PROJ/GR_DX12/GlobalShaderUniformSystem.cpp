#include "GlobalShaderUniformSystem.h"

#include <cassert>
#include <glm/gtc/type_ptr.hpp>

#include "glm/glm.hpp"
#include "glm/fwd.hpp"
#include "ShaderUniform.h"
#include "Graphics.h"
#include "d3dx12.h"
#include "Scene.h"
#include "ThirdCamera.h"


GlobalShaderUniformSystem* GlobalShaderUniformSystem::Instance = nullptr;

GlobalShaderUniformSystem::GlobalShaderUniformSystem()
{
	assert(Instance == nullptr);
	Instance = this;
}


void GlobalShaderUniformSystem::Initialize()
{	
	//HHQ_MATRIX_V; HHQ_MATRIX_P;
	auto device = DX12Graphics::Instance->GetDevice();
	assert(device != nullptr);
	
	//添加全局的 View视图矩阵
	{
		ComPtr<ID3D12Resource> viewResource;
		auto heapPropertiesDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(glm::mat4));
		device->CreateCommittedResource(&heapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(viewResource.GetAddressOf()));
		m_GlobalUniformBufferMap.emplace("HHQ_MATRIX_V", viewResource);

		ShaderUniform viewUniform;
		viewUniform.UniformType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		viewUniform.BufferLocation = viewResource->GetGPUVirtualAddress();

		m_GlobalUniformMap.emplace("HHQ_MATRIX_V", viewUniform);
	}

	//添加全局的Perspective矩阵
	{
		ComPtr<ID3D12Resource> porjResource;
		auto heapPropertiesDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(glm::mat4));
		device->CreateCommittedResource(&heapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(porjResource.GetAddressOf()));
		m_GlobalUniformBufferMap.emplace("HHQ_MATRIX_P", porjResource);

		ShaderUniform projUniform;
		projUniform.UniformType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		projUniform.BufferLocation = porjResource->GetGPUVirtualAddress();

		m_GlobalUniformMap.emplace("HHQ_MATRIX_P", projUniform);
	}
}

GlobalShaderUniformSystem::~GlobalShaderUniformSystem()
{
	m_GlobalUniformBufferMap.clear();
	m_GlobalUniformMap.clear();
}


void GlobalShaderUniformSystem::Update()
{
	ThirdCamera* camera = Scene::Default->GetSceneCamera();

	{
		byte* dst = nullptr;
		m_GlobalUniformBufferMap["HHQ_MATRIX_V"]->Map(0, nullptr, (void**)&dst);
		glm::mat4 viewMat = camera->GetViewMatrix();
		float* src = glm::value_ptr(viewMat);
		memcpy(dst, src, sizeof(glm::mat4));
		m_GlobalUniformBufferMap["HHQ_MATRIX_V"]->Unmap(0, nullptr);
	}

	{
		byte* dst = nullptr;
		m_GlobalUniformBufferMap["HHQ_MATRIX_P"]->Map(0, nullptr, (void**)&dst);
		glm::mat4 projMat = camera->GetPerspectiveMatrix();
		float* src = glm::value_ptr(projMat);
		memcpy(dst, src, sizeof(glm::mat4));
		m_GlobalUniformBufferMap["HHQ_MATRIX_P"]->Unmap(0, nullptr);
	}
}

bool GlobalShaderUniformSystem::GetShaderUniform(std::string uniform_name, ShaderUniform* ret) const
{
	auto iter = m_GlobalUniformMap.find(uniform_name);
	if (iter == m_GlobalUniformMap.end())
		return false;
	
	*ret = iter->second;
	return true;
}





