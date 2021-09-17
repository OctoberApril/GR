#include "Model.h"

void Model::LoadFileFromPath(const std::string& path)
{
	Assimp::Importer importer;
	auto pScene = importer.ReadFile(path, aiProcess_Triangulate);

	RecursiveNode(pScene, pScene->mRootNode);
}

void Model::RecursiveNode(const aiScene* scene, const aiNode* node)
{
	int indiceStartIndex = m_Vertices.size();
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		auto mesh = scene->mMeshes[node->mMeshes[i]];

		for (int j = 0; j < mesh->mNumVertices; j++)
		{
			VertexData vertex;
			if (mesh->HasPositions())
				vertex.position = float3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
			if (mesh->HasVertexColors(0))
				vertex.color = float4(mesh->mColors[0][j].r, mesh->mColors[0][j].g, mesh->mColors[0][j].b, mesh->mColors[0][j].a);
			if (mesh->HasNormals())
				vertex.normal = float3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z);
			if (mesh->HasTextureCoords(0))
				vertex.uv0 = float2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y);
			m_Vertices.push_back(vertex);
		}

		for (int j = 0; j < mesh->mNumFaces; j++)
		{
			auto face = mesh->mFaces[j];
			for (int k = 0; k < face.mNumIndices; k++)
			{
				m_Indices.push_back(face.mIndices[k] + indiceStartIndex);
			}
		}
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		RecursiveNode(scene, node->mChildren[i]);
	}
}