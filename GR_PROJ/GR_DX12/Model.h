#pragma once
#include <vector>
#include <string>

#include "float2.h"
#include "float3.h"
#include "float4.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"


struct VertexData
{
	VertexData() = default;
	VertexData(const VertexData& other)
	{
		this->position = other.position;
		this->color = other.color;
		this->normal = other.normal;
		this->uv0 = other.uv0;
	}

	VertexData& operator=(const VertexData& other)
	{
		this->position = other.position;
		this->color = other.color;
		this->normal = other.normal;
		this->uv0 = other.uv0;

		return *this;
	}

	float3 position;
	float3 normal;
	float4 color;
	float2 uv0;
};

class Model
{
public:
	Model() = default;
	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

	void LoadFileFromPath(const std::string& path);
	std::vector<VertexData> GetVertexs() { return m_Vertices; }
	std::vector<uint32_t> GetIndices() { return m_Indices; }

private:

	void RecursiveNode(const aiScene* scene,const aiNode*);

	std::vector<VertexData> m_Vertices;
	std::vector<uint32_t> m_Indices;
};

