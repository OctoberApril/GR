#include "Material.h"
#include "HPass.h"
#include "Texture2D.h"
#include <glm/detail/type_mat4x4.hpp>

Material::Material(const wchar_t* vsShader, const wchar_t* psShader) : m_pHPass(std::move(new HPass(vsShader, psShader)))
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
	if (m_pHPass != nullptr)
	{
		if (!m_pHPass->CheckVariableIsValidate(variableName,sizeof(matrix)))
		{
			char* buffer = new char[256];
			sprintf_s(buffer, 256, "Shader中不存在变量:%s或者类型不一致,验证失败.", variableName);
			OutputDebugStringA(buffer);
		}
	}
	m_MatrixVariableMap.emplace(variableName, matrix);
}

void Material::SetTexture2D(std::string variableName, Texture2D* texture)
{
	if (m_pHPass != nullptr)
	{
		if (!m_pHPass->CheckVariableIsValidate(variableName))
		{
			char* buffer = new char[256];
			sprintf_s(buffer,256,"Shader中不存在变量:%s,验证失败.",variableName);
			OutputDebugStringA(buffer);
		}
	}
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



