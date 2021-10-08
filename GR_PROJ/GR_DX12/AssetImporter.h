#pragma once

class GameObject;
struct aiScene;
struct aiNode;


class AssetImporter
{
public:
	static GameObject* ImportModel(const char* path);
private:
	static void RecursiveModelNode(GameObject* parent, const aiScene* scene, const aiNode* node);
};

