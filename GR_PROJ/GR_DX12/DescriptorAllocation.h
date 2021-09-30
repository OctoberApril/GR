#pragma once
#include <d3d12.h>

class DescriptorAllocationPage;

class DescriptorAllocation
{
public:
	DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, size_t length, uint32_t incremental)
		:CPU(cpuHandle), Length(length), IncrementInDescriptorHeap(incremental)
	{
		
	}

	D3D12_CPU_DESCRIPTOR_HANDLE CPU;

	//D3D12_GPU_DESCRIPTOR_HANDLE GPU;

	size_t Length;

	uint32_t IncrementInDescriptorHeap;

	bool operator <(const DescriptorAllocation& rhs) const
	{
		return Length < rhs.Length;
	}

	bool operator ==(const size_t& rhs) const
	{
		return Length == rhs;
	}

private:
	friend class DescriptorAllocationPage;

	SIZE_T parent;
	SIZE_T next;
};

