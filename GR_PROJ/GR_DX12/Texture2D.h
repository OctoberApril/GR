#pragma once
#include "HObject.h"
#include <d3d12.h>
#include <unordered_map>
#include "DirectXTex/DirectXTex.h"


static InterfaceID IID_TEXTURE2D = { 4,5,0,{0,0,0,0,0,0,0,0} };


class Texture2D : public HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_TEXTURE2D, HObject)
public:
	Texture2D();
	Texture2D(const wchar_t* imagePath);
	~Texture2D();
	
	unsigned char* GetImageData() const;

	ComPtr<ID3D12Resource> GetResource() const;
private:
	void CreateImageResource();
private:

	int m_ImageWidth;
	int m_ImageHeight;
	int m_ImageChannel;

	DirectX::TexMetadata m_Meta;
	DirectX::ScratchImage m_Image;
	
	unsigned char* m_ImageData;
	
	ComPtr<ID3D12Resource> m_Resource;

	//static std::unordered_map<std::string, ID3D12Resource*> g_TexResMap;
};

