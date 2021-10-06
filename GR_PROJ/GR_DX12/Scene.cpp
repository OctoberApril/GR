#include "Scene.h"
#include "GameObject.h"

Scene* Scene::Default(new Scene("default scene"));

InterfaceID Scene::GetIID()
{
	return IID_SCENE;
}

Scene::~Scene()
{
	m_RootGameObjects.clear();	
}

Scene::Scene(std::string scene_name) :m_SceneName(scene_name)
{
	
}

std::vector<GameObject*> Scene::GetRootGameObjects() const
{	
	std::vector<GameObject*> result;
	for(auto p = m_RootGameObjects.begin();p != m_RootGameObjects.end();p++)
	{
		result.emplace_back(p->Get());
	}
	return result;
}

void Scene::AddGameObjectToSceneRoot(GameObject* go)
{
	m_RootGameObjects.emplace_back(SharedPtr<GameObject>(go));
}




