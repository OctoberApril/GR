#pragma once
#include "Component.h"

static const InterfaceID IID_DRAWABLE = { 4,0,0,{0,0,0,0,0,0,0,0} };

class Drawable : public Component
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_DRAWABLE, Component)
};

