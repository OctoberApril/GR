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
	~Scene();

	Transform* GetRootTransform() const;

	static InterfaceID GetIID();

public:
	static Scene* Default;
private:

	std::string m_SceneName;

	Transform* m_pRootTransform;
};



