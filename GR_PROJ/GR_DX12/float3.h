#pragma once

struct float3
{
	float3() :x(0), y(0), z(0) {}
	float3(float _x, float _y, float _z) :x(_x), y(_y), z(_z) {};
	float3(const float3& other)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
	}

	float3& operator=(const float3& other)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		return *this;
	}

	float3 operator-(const float3& other)
	{
		float _x = this->x - other.x;
		float _y = this->y - other.y;
		float _z = this->z - other.z;

		return float3(_x, _y, _z);
	}

	float3 operator+(const float3& other)
	{
		float _x = this->x + other.x;
		float _y = this->y + other.y;
		float _z = this->z + other.z;

		return float3(_x, _y, _z);
	}

	float operator*(const float3& other)
	{
		return x * other.x + y * other.y + z * other.z;
	}

	float3 CrossProduct(const float3& other) const
	{
		float _x = this->y * other.z - this->z * other.y;
		float _y = this->z * other.x - this->x * other.z;
		float _z = this->x * other.y - this->y * other.x;

		return float3(_x, _y, _z);
	}

	float3 Normalized()
	{
		float length = sqrt(x * x + y * y + z * z);
		float _x = this->x / length;
		float _y = this->y / length;
		float _z = this->z / length;

		return float3(_x, _y, _z);
	}

	float x;
	float y;
	float z;
};