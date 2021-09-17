#pragma once

struct float2
{
	float2() :x(0), y(0){}
	float2(float _x, float _y) :x(_x), y(_y){};
	float2(const float2& other)
	{
		this->x = other.x;
		this->y = other.y;
	}

	float2& operator=(const float2& other)
	{
		this->x = other.x;
		this->y = other.y;
		return *this;
	}

	float2 operator+(const float2& other) const
	{
		float _x = this->x + other.x;
		float _y = this->y + other.y;

		return float2(_x, _y);
	}

	float2 operator-(const float2& other) const
	{
		float _x = this->x - other.x;
		float _y = this->y - other.y;

		return float2(_x, _y);
	}

	float x;
	float y;
};