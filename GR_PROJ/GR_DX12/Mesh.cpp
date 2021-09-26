#include "Mesh.h"


void Mesh::SetVectices(std::vector<glm::vec3> vertices)
{	
	m_Vertices = vertices;
}

void Mesh::SetColors(std::vector<glm::vec4> colors)
{
	m_Colors = colors;
}

void Mesh::SetUv0s(std::vector<glm::vec2> uv0s)
{
	m_Uv0s = uv0s;
}

void Mesh::SetNormals(std::vector<glm::vec3> normals)
{
	m_Normals = normals;
}

void Mesh::SetIndices(std::vector<uint32_t> indices)
{
	m_Indices = indices;
}

std::vector<glm::vec3> Mesh::GetVertices() const
{
	return m_Vertices;
}

std::vector<glm::vec4> Mesh::GetColors() const
{
	return m_Colors;
}

std::vector<glm::vec2> Mesh::GetUv0s() const
{
	return m_Uv0s;
}

std::vector<glm::vec3> Mesh::GetNormals() const
{
	return m_Normals;
}

std::vector<uint32_t> Mesh::GetIndices() const
{
	return m_Indices;
}







