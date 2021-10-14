#pragma once
#include <d3d12.h>
#include <cstdint>

class DescriptorAllocationPage;

class DescriptorAllocation
{
public:
	DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle, size_t length, uint32_t incremental, DescriptorAllocationPage* owner = nullptr)
		:CPU(cpuHandle), GPU(gpuHandle), Length(length), IncrementInDescriptorHeap(incremental), m_Owner(owner)
	{

	}

	D3D12_CPU_DESCRIPTOR_HANDLE CPU;
	D3D12_GPU_DESCRIPTOR_HANDLE GPU;
	
	size_t Length;

	uint32_t IncrementInDescriptorHeap;

	void Release(uint64_t frame_num) const;
	
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

	DescriptorAllocationPage* m_Owner;
};

