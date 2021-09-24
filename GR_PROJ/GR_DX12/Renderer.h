#pragma once
#include "Drawable.h"

static const InterfaceID IID_RENDERER = { 4,1,0,{0,0,0,0,0,0,0,0} };

class Renderer : public Drawable
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_RENDERER, Drawable)
};

