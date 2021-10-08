#include "Transform.h"
#include "GameObject.h"
#include "SharedPtr.h"
#include "Scene.h"

InterfaceID GameObject::GetIID()
{
	return IID_GAMEOBJECT;
}

GameObject::GameObject() :m_pTransform(new Transform(this))
{

}

GameObject::GameObject(GameObject* parent): m_pTransform(new Transform(this, parent))
{
	
}


GameObject::~GameObject()
{
	for (int i = 0; i < m_Comps.size(); i++)
	{
		m_Comps[i].Release();
	}
	
	delete m_pTransform;
}

Transform* GameObject::transform() const
{
	return m_pTransform;
}

void GameObject::AddComponent(Component* cmp)
{
	SharedPtr<Component> newCmp(cmp);
	m_Comps.push_back(newCmp);
}

void GameObject::Destroy(SharedPtr<Component> cpt)
{
	if (cpt == nullptr) return;
	if (cpt->m_pGameObject == nullptr) return;

	GameObject* go = cpt->m_pGameObject;
	for (auto i = go->m_Comps.begin(); i != go->m_Comps.end(); i++)
	{
		if (*i == cpt)
		{
			(*i).Release();
			go->m_Comps.erase(i);
		}
	}
}



