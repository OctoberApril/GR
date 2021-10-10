#include "Scene.h"
#include "GameObject.h"
#include "ThirdCamera.h"


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

void Scene::RemoveGameObjectFromScene(GameObject* go)
{
	for(auto p = m_RootGameObjects.begin();p != m_RootGameObjects.end();p++)
	{
		if(*p == go)
		{
			m_RootGameObjects.erase(p);
			return;
		}
	}
}

ThirdCamera* Scene::GetSceneCamera() const
{
	return m_ThirdCamera;
}

void Scene::SetSceneCamera(ThirdCamera* camera)
{
	if (camera == nullptr) return;
	m_ThirdCamera = camera;
}






