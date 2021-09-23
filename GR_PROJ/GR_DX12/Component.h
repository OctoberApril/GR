#pragma once
#include "HObject.h"

static const InterfaceID IID_COMPONENT = { 2,0,0,{0,0,0,0,0,0,0,0} };

class GameObject;
class Transform;

class Component : public HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_COMPONENT, HObject)
public:
	Component() :m_pGameObject(nullptr), m_pTransform(nullptr) {}

	static InterfaceID GetIID();

protected:
	friend class GameObject;

	GameObject* m_pGameObject;
	Transform* m_pTransform;
};

