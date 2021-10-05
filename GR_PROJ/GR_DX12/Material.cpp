#include "Material.h"
#include "HPass.h"
#include "Texture2D.h"
#include <glm/fwd.hpp>
#include <glm/detail/type_mat4x4.hpp>

Material::Material(const wchar_t* vsShader, const wchar_t* psShader) : m_pHPass(std::move(new HPass(vsShader, psShader)))
{

}

Material::~Material()
{	
	delete m_pHPass;
}

