#include "Texture2D.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <cassert>


Texture2D::Texture2D() :m_ImageData(nullptr)
{

}

Texture2D::Texture2D(const char* imagePath)
{
	assert(imagePath != nullptr);
	m_ImageData = stbi_load(imagePath, &m_ImageWidth, &m_ImageHeight, &m_ImageChannel, 0);
}

Texture2D::~Texture2D()
{
	if (!m_ImageData)
		stbi_image_free(m_ImageData);
}

unsigned char* Texture2D::GetImageData() const
{
	return m_ImageData;
}


