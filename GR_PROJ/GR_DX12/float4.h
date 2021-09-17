#pragma once

struct float4
{
	float4() :x(0), y(0), z(0),w(0) {}
	float4(float _x, float _y, float _z,float _w) :x(_x), y(_y), z(_z),w(_w) {};
	float4(const float4& other)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		this->w = other.w;
	}

	float4& operator=(const float4& other)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		this->w = other.w;
		return *this;
	}

	float x;
	float y;
	float z;
	float w;
};