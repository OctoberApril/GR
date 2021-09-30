#include "DescriptorAllocator.h"
#include "DescriptorAllocationPage.h"
#include "DescriptorAllocation.h"

DescriptorAllocator::DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE heap_type, int default_descriptor_num)
{
	m_DescriptorHeapType = heap_type;
	m_NumberPerHeapDescriptor = default_descriptor_num;
}

D3D12_DESCRIPTOR_HEAP_TYPE DescriptorAllocator::GetDescriptorType() const
{
	return m_DescriptorHeapType;
}

DescriptorAllocation DescriptorAllocator::Allocate(int descriptor_num)
{
	for(auto i = m_DescriptorPages.begin();i != m_DescriptorPages.end();i++)
	{
		if((*i)->HasSpace(descriptor_num))
		{
			return (*i)->Allocate(descriptor_num);
		}
	}

	auto newPage = CreateDescriptorAllocatorPage();
	return newPage->Allocate(descriptor_num);
}

DescriptorAllocationPage* DescriptorAllocator::CreateDescriptorAllocatorPage()
{
	std::shared_ptr<DescriptorAllocationPage> newPage = std::make_shared<DescriptorAllocationPage>(m_DescriptorHeapType,m_NumberPerHeapDescriptor);

	m_DescriptorPages.emplace_back(newPage);

	return newPage.get();
}

void DescriptorAllocator::ReleaseStaleDescriptors(uint64_t frame) const
{
	for(auto p = m_DescriptorPages.begin();p != m_DescriptorPages.end();p++)
	{
		(*p)->Release(frame);
	}
}







