#pragma once
#include <string>

#include "HObject.h"
class Transform;


static const InterfaceID IID_SCENE = { 3,0,0,{0,0,0,0,0,0,0,0} };

class Scene : public HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_SCENE, HObject)
public:
	Scene(std::string scene_name = "");
	Scene(const Scene&) = delete;
	Scene(Scene&&) = delete;
	Scene& operator=(const Scene&) = delete;
	Scene& operator=(Scene&&) = delete;
	
	~Scene() = default;

	SharedPtr<Transform> GetRootTransform() const;

public:
	static SharedPtr<Scene> Default;
private:

	std::string m_SceneName;

	SharedPtr<Transform> m_RootTransform;
};



