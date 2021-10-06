#pragma once
#include <string>
#include <vector>
#include "HObject.h"

static const InterfaceID IID_SCENE = { 3,0,0,{0,0,0,0,0,0,0,0} };

class GameObject;

class Scene : public HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_SCENE, HObject)
public:
	Scene(std::string scene_name);
	~Scene() override;

	std::vector<GameObject*> GetRootGameObjects() const;
	void AddGameObjectToSceneRoot(GameObject* go);
	
	static InterfaceID GetIID();
	static Scene* Default;
private:

	std::string m_SceneName;
	std::vector<SharedPtr<GameObject>> m_RootGameObjects;
};



