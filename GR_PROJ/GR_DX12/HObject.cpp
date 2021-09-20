#include "HObject.h"


void HObject::AddRef()
{
	++m_RefCount;
}

void HObject::Release()
{
	--m_RefCount;
}

int HObject::GetRefCount()
{
	return m_RefCount;
}

void HObject::QueryInterface(const InterfaceID& iid, HObject** object)
{
	if (object == nullptr) return;

	if(iid == IID_OBJECT)
	{
		this->AddRef();
		*object = this;
	}
}

InterfaceID HObject::GetIID()
{
	return IID_OBJECT;
}



