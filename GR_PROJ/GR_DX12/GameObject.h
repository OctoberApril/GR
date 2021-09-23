#pragma once
#include <vector>
#include "HObject.h"


static const InterfaceID IID_GAMEOBJECT = { 1,0,0,{0,0,0,0,0,0,0,0} };

class Transform;
class Component;

class GameObject : public HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_GAMEOBJECT, HObject)
public:
	GameObject();
	virtual ~GameObject();

	Transform* transform() const;	
	void AddComponent(Component* cmp);

	template<typename T> T* GetComponent();
	template<typename T> void AddComponent();
	
	static void Destroy(SharedPtr<Component> cpt);
	static InterfaceID GetIID();

private:

	std::vector<SharedPtr<Component>> m_Comps;
	Transform* m_pTransform;
};


template <typename T>
T* GameObject::GetComponent()
{
	T* result = nullptr; 
	for(int i = 0;i < m_Comps.size();i++)
	{
		m_Comps[i]->QueryInterface(T::GetIID(), &result);
		if (result != nullptr) break;			
	}

	return result;
}

template<typename T>
void GameObject::AddComponent()
{	
	SharedPtr<T> newObj = new T();
	m_Comps.push_back(newObj);
}


