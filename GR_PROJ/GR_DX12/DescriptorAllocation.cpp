#include "DescriptorAllocation.h"
#include "DescriptorAllocationPage.h"

void DescriptorAllocation::Release(uint64_t frame_num) const
{
	if(!m_Owner)
	{
		m_Owner->Free(*this, frame_num);
	}
}
