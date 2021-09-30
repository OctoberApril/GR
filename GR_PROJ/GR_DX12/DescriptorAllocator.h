#pragma once
#include <d3d12.h>
#include <vector>
#include <memory>

class DescriptorAllocationPage;
class DescriptorAllocation;

class DescriptorAllocator
{
public:
	DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE heap_type, int default_descriptor_num = 256);
	~DescriptorAllocator() = default;

	D3D12_DESCRIPTOR_HEAP_TYPE GetDescriptorType() const;

	DescriptorAllocation Allocate(int descriptor_num);
	
	void ReleaseStaleDescriptors(uint64_t frame) const;
private:

	DescriptorAllocationPage* CreateDescriptorAllocatorPage();

private:

	template<typename T> using SharedPtr = std::shared_ptr<T>;
		
	std::vector<SharedPtr<DescriptorAllocationPage>> m_DescriptorPages;
	
	D3D12_DESCRIPTOR_HEAP_TYPE m_DescriptorHeapType;

	int m_NumberPerHeapDescriptor;
};

