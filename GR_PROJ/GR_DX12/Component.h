#pragma once
#include "HObject.h"

static const InterfaceID IID_COMPONENT = { 2,0,0,{0,0,0,0,0,0,0,0} };

class Component : public HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_COMPONENT, HObject)
	IMPLEMENT_DEFAULT_CONSTRUCTS(Component)
public:

};

