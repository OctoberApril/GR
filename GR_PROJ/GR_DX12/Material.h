#pragma once
#include <wrl.h>
#include "HObject.h"
#include<unordered_map>
#include<string>
#include <glm/fwd.hpp>


static const InterfaceID IID_MATERIAL = { 4,4,0,{0,0,0,0,0,0,0,0} };

class HPass;
class Texture2D;

class Material : public HObject
{
public:
	IMPLEMENT_QUERY_INTERFACE_INPLACE(IID_MATERIAL, HObject)
public:
	template<typename T>  using ComPtr = Microsoft::WRL::ComPtr<T>;
	Material(const wchar_t* vsShader, const wchar_t* psShader);
	~Material();



private:
	HPass* m_pHPass;

	std::unordered_map<std::string, glm::mat4> m_MatrixVariableMap;
	std::unordered_map<std::string, Texture2D> m_Texture2DVariableMap;
};

