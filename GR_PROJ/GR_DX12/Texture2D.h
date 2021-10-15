#pragma once
#include "HObject.h"
#include <d3d12.h>


static InterfaceID IID_TEXTURE2D = { 4,5,0,{0,0,0,0,0,0,0,0} };

class Texture2D : public HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_TEXTURE2D, HObject)
public:
	Texture2D();
	Texture2D(const char* imagePath);
	~Texture2D();
	
	unsigned char* GetImageData() const;

	ComPtr<ID3D12Resource> GetResource() const;
private:
	void CreateImageResource();
private:

	int m_ImageWidth;
	int m_ImageHeight;
	int m_ImageChannel;
	
	unsigned char* m_ImageData;
	
	ComPtr<ID3D12Resource> m_Resource;
};

