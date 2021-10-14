#include "Material.h"

#include <algorithm>

#include "HPass.h"
#include "Texture2D.h"
#include <glm/detail/type_mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Graphics.h"
#include "UploadBuffer.h"
#include "GlobalShaderUniformSystem.h"
#include "ShaderUniform.h"
#include "ThirdCamera.h"
#include "DescriptorAllocation.h"



Material::Material(const wchar_t* vsShader, const wchar_t* psShader) : m_pHPass(std::move(new HPass(vsShader, psShader)))
{
	InitRootSignatureResource();
}

Material::~Material()
{

	delete m_pHPass;
}

void Material::SetMatrix(std::string variableName, glm::mat4 matrix)
{

}

void Material::SetTexture2D(std::string variableName, Texture2D* texture)
{

}

ID3D12PipelineState* Material::GetPipelineStateObject() const
{
	return m_pHPass->GetPipelineStateObject();
}

ID3D12RootSignature* Material::GetRootSignature() const
{
	return m_pHPass->GetRootSignature();
}

void Material::BindResources()
{
	//子类绑定相关Shader中需要的资源
}

void Material::InitRootSignatureResource()
{
	auto device = DX12Graphics::Instance->GetDevice();
	assert(device != nullptr);

	auto rsParameters = m_pHPass->GetPassRootSignatureParameters();
	std::unordered_map<int, std::vector<RootSignatureParameter>> paramsSet;

	for (auto p = rsParameters.begin(); p != rsParameters.end(); p++)
	{
		paramsSet[p->second.ParameterIndex].emplace_back(p->second);
	}

	for (auto p = paramsSet.begin(); p != paramsSet.end(); p++)
	{
		if (p->second.size() > 1)
		{
			std::sort(p->second.begin(), p->second.end(), [](const RootSignatureParameter& lhs, const RootSignatureParameter& rhs)
				{
					return lhs.DescriptorTable.Offset < rhs.DescriptorTable.Offset;
				});
		}
	}

	//初始化 m_RootSignatureBindItems m_ShaderUniformMap
	for (auto p = paramsSet.begin(); p != paramsSet.end(); p++)
	{
		auto info = p->second[0];
		RootSignatureTableBindItem item = {};
		item.ParameterIndex = info.ParameterIndex;
		item.ParameterType = info.RootParameterType;

		if (info.RootParameterType != D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			if (s_LocalShaderUniform.count(info.VariableName) != 0) continue;

			ShaderUniform su = {};
			if (GlobalShaderUniformSystem::Instance->GetShaderUniform(info.VariableName, &su))
			{
				item.BufferLocation = su.BufferLocation;
			}
			else
			{
				su.UniformType = info.RootParameterType;
			}
			m_ShaderUniformMap.emplace(info.VariableName, su);
			m_RootSignatureBindItems.emplace_back(item);
		}
		else
		{
			int sum = 0;
			for (auto m = p->second.begin(); m != p->second.end(); m++)
			{
				sum += m->DescriptorTable.Size;
			}

			auto allocation = DX12Graphics::Instance->AllocateDescriptors(info.DescriptorTable.HeapType, sum);
			item.DescriptorHandle = allocation.GPU;
			m_RootSignatureBindItems.emplace_back(item);

			for (auto m = p->second.begin(); m != p->second.end(); m++)
			{
				ShaderUniform su = {};
				su.UniformType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				su.CpuDescriptorHandle = allocation.CPU;
				m_ShaderUniformMap.emplace(m->VariableName, su);

				allocation.CPU.ptr += m->DescriptorTable.DescriptorIncremental * m->DescriptorTable.Size;
			}
		}
	}
}

std::vector<RootSignatureTableBindItem> Material::GetRootSignatureTable() const
{
	return m_RootSignatureBindItems;
}

int Material::GetParameterIndex(std::string uniform_name) const
{
	auto variableMap = m_pHPass->GetPassRootSignatureParameters();
	auto iter = variableMap.find(uniform_name);
	if (iter != variableMap.end())
		return iter->second.ParameterIndex;
	return -1;
}







