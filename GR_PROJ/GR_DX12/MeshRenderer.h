#pragma once
#include "Renderer.h"


static const InterfaceID IID_MESHRENDERER = { 4,2,0,{0,0,0,0,0,0,0,0} };

class MeshRenderer : public Renderer
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_MESHRENDERER, Renderer)
	
};

