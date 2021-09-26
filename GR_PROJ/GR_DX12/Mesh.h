#pragma once
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "HObject.h"

static const InterfaceID IID_MESH = { 4,3,0,{0,0,0,0,0,0,0,0} };

class Mesh : public HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_MESH, HObject)
public:
	Mesh() = default;
	virtual ~Mesh() = default;

	void SetVectices(std::vector<glm::vec3> vertices);
	void SetColors(std::vector<glm::vec4> colors);
	void SetUv0s(std::vector<glm::vec2> uv0s);
	void SetNormals(std::vector<glm::vec3> normals);
	void SetIndices(std::vector<uint32_t> indices);
	
	std::vector<glm::vec3> GetVertices() const;
	std::vector<glm::vec4> GetColors() const;
	std::vector<glm::vec2> GetUv0s() const;
	std::vector<glm::vec3> GetNormals() const;
	std::vector<uint32_t>  GetIndices() const;

private:
	//position uv0  color normal
	std::vector<glm::vec3> m_Vertices;
	std::vector<glm::vec2> m_Uv0s;
	std::vector<glm::vec4> m_Colors;
	std::vector<glm::vec3> m_Normals;
	std::vector<uint32_t> m_Indices;
};

