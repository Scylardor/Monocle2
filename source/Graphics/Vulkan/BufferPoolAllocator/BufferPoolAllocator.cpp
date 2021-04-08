#ifdef MOE_VULKAN

#include "BufferPoolAllocator.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"

namespace moe
{
	void BufferPoolAllocator::Initialize(MyVkDevice& device, uint64_t blockSize, uint64_t blockCount, vk::BufferUsageFlagBits usageFlags)
	{
		m_device = &device;

		auto alignment = device.GetMinimumBufferBlockAlignment(usageFlags);
		m_alignedBlockSize = VulkanBuffer::Align(blockSize, alignment);

		m_poolSize = blockCount * m_alignedBlockSize;

		m_bufferPool = device.BufferAllocator().Create(blockCount * m_alignedBlockSize, usageFlags, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		m_bufferMap = m_bufferPool.Map(device);

		m_freeHandles.reserve(blockCount);
		m_freeHandles.push_back({m_bufferPool, 0, m_alignedBlockSize });
	}


	BufferHandle BufferPoolAllocator::AllocateBlock()
	{
		if (false == m_freeHandles.empty())
		{
			BufferHandle allocHandle = m_freeHandles.back();
			m_freeHandles.pop_back();
			return allocHandle;
		}

		// try to push high water mark and return new offset
		m_nextConsumedOffset += m_alignedBlockSize;
		if (m_nextConsumedOffset == m_poolSize)
		{
			// Pool exhausted ; we need to allocate a new one
			MOE_ASSERT(false);
			return BufferHandle();
		}
		// else just return an handle to the new high water mark
		BufferHandle newHandle{ m_bufferPool, m_nextConsumedOffset, m_alignedBlockSize };
		return newHandle;
	}

	void BufferPoolAllocator::FreeBlock(BufferHandle block)
	{
		// Just add this buffer handle to the free list
		m_freeHandles.push_back(block);
	}

}


#endif // MOE_VULKAN
