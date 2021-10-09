#pragma once
#include <cstdint>
#include <cstring>

struct InterfaceID
{
	uint32_t Data1;
	uint16_t Data2;
	uint16_t Data3;
	uint8_t  Data4[8];

	bool operator==(const InterfaceID& other) const
	{
		return this->Data1 == other.Data1 &&
			this->Data2 == other.Data2 &&
			this->Data3 == other.Data3 &&
			memcmp((void*)Data4, other.Data4, sizeof(Data4)) == 0;
	}
};