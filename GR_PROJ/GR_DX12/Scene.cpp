#include "Scene.h"
#include "Transform.h"

Scene* Scene::Default(new Scene("default scene"));

InterfaceID Scene::GetIID()
{
	return IID_SCENE;
}

Scene::~Scene()
{
	if (m_pRootTransform != nullptr) delete m_pRootTransform;
}

Scene::Scene(std::string scene_name) :m_SceneName(scene_name)
{
	m_pRootTransform = new Transform(nullptr,nullptr);
}

Transform* Scene::GetRootTransform() const
{
	return m_pRootTransform;
}


