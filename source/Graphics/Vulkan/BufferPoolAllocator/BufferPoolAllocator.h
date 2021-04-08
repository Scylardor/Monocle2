#pragma once
#include "Graphics/Vulkan/Buffer/VulkanBuffer.h"
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

namespace moe
{
	struct BufferHandle
	{
		//uint16_t		PoolIdx{ 0 };
		vk::Buffer		Buffer{};
		vk::DeviceSize	Offset{0};
		vk::DeviceSize	Range{0};
	};

	class BufferPoolAllocator
	{
	public:


		void	Initialize(MyVkDevice& device, uint64_t blockSize, uint64_t blockCount, vk::BufferUsageFlagBits usageFlags);

		BufferHandle	AllocateBlock();

		void			FreeBlock(BufferHandle block);

		template <typename TMap>
		TMap*	MapBufferAs()
		{
			return m_bufferMap.As<TMap>();
		}


	protected:

	private:

		const MyVkDevice& Device() const
		{
			MOE_ASSERT(m_device != nullptr);
			return *m_device;
		}

		const MyVkDevice* m_device = nullptr;

		// The second buffer is used when the first one became too small and we had to reallocate.
		VulkanBuffer				m_bufferPool;
		std::vector<BufferHandle>	m_freeHandles;
		void*						m_bufferMap{nullptr};

		uint64_t	m_alignedBlockSize = 0;
		uint64_t	m_poolSize = 0;
		uint64_t	m_nextConsumedOffset = 0; // "High water mark" of the consumption of the pool. When no handles are available, we push the high water mark. If it's at the limit, we allocate a new pool.

	};
}


#endif // MOE_VULKAN
