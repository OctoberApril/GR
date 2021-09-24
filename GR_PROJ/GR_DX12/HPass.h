#pragma once
#include "HObject.h"

static const InterfaceID IID_HPASS = { 5,0,0,{0,0,0,0,0,0,0,0} };

class HPass : HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_HPASS,HObject)
public:

	
};

