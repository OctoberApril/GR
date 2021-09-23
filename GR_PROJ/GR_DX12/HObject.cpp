#include <exception>
#include "HObject.h"
#include <cassert>


HObject::~HObject()
{
	assert(m_RefCount == 0);
}


void HObject::AddRef()
{
	++m_RefCount;
}

void HObject::Release()
{
	--m_RefCount;
	if (m_RefCount == 0) delete this;
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



