#include "AssetImporter.h"

#include "DefaultMaterial.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Transform.h"


GameObject* AssetImporter::ImportModel(const char* path)
{
	Assimp::Importer importer;
	auto pScene = importer.ReadFile(path, aiProcess_Triangulate);

	GameObject* go = new GameObject();
	RecursiveModelNode(go, pScene, pScene->mRootNode);
	return go;
}

void AssetImporter::RecursiveModelNode(GameObject* parent, const aiScene* scene, const aiNode* node)
{
	GameObject* go = new GameObject(parent);
	if(parent != nullptr)
	{
		parent->transform()->AddChild(go->transform());
	}
	
	Mesh* mesh = go->AddComponent<Mesh>();
	MeshRenderer* renderer = go->AddComponent<MeshRenderer>();
	DefaultMaterial* mat = new DefaultMaterial();
	renderer->SetMaterial(mat);
	
	std::vector<glm::vec3>* vertices = new std::vector<glm::vec3>();
	std::vector<glm::vec2>* uv0 = new std::vector<glm::vec2>();
	std::vector<glm::vec4>* colors = new std::vector<glm::vec4>();
	std::vector<glm::vec3>* normals = new std::vector<glm::vec3>();
	std::vector<uint32_t>* indices = new std::vector<uint32_t>();
	
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		auto mesh = scene->mMeshes[node->mMeshes[i]];

		for (int j = 0; j < mesh->mNumVertices; j++)
		{
			if (mesh->HasPositions())
				vertices->push_back(glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z));

			if (mesh->HasVertexColors(0))
				colors->push_back(glm::vec4(mesh->mColors[0][j].r, mesh->mColors[0][j].g, mesh->mColors[0][j].b, mesh->mColors[0][j].a));

			if (mesh->HasNormals())
				normals->push_back(glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z));

			if (mesh->HasTextureCoords(0))
				uv0->push_back(glm::vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y));
		}

		for (int j = 0; j < mesh->mNumFaces; j++)
		{
			auto face = mesh->mFaces[j];
			for (int k = 0; k < face.mNumIndices; k++)
			{
				indices->push_back(face.mIndices[k]);
			}
		}
	}

	mesh->SetVectices(*vertices);
	mesh->SetColors(*colors);
	mesh->SetUv0s(*uv0);
	mesh->SetNormals(*normals);
	mesh->SetIndices(*indices);

	
	for (int i = 0; i < node->mNumChildren; i++)
	{
		RecursiveModelNode(go,scene, node->mChildren[i]);
	}
}

