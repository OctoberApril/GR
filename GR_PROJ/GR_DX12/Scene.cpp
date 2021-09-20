#include "Scene.h"
#include "Transform.h"

SharedPtr<Scene> Scene::Default(new Scene("default scene"));


Scene::Scene(std::string scene_name) :m_SceneName(scene_name)
{
	m_RootTransform = new Transform();
}

SharedPtr<Transform> Scene::GetRootTransform() const
{
	return m_RootTransform;
}


