#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"
#include "Graphics/Vulkan/CommandPool/VulkanCommandPool.h"

namespace moe
{
	class MyVkDevice;

	class VulkanCommandPipeline
	{
	public:
		inline static const auto DEFAULT_BUFFERS_PER_POOL = 32u;

		~VulkanCommandPipeline();

		bool	Initialize(const MyVkDevice& device, vk::CommandBufferLevel cbLevel = vk::CommandBufferLevel::ePrimary,
							vk::CommandPoolCreateFlagBits flags = vk::CommandPoolCreateFlagBits(),
							uint32_t reservedPools = 1, uint32_t reservedBuffersPerPool = DEFAULT_BUFFERS_PER_POOL);


		void	Reset(const MyVkDevice& device);

		vk::CommandBuffer	GrabCommandBuffer(const MyVkDevice& device);

	protected:

	private:

		const MyVkDevice* m_device = nullptr;

		const MyVkDevice& Device() const
		{
			MOE_ASSERT(m_device != nullptr);
			return *m_device;
		}


		vk::CommandBuffer	GrabAvailableBuffer();

		std::vector<VulkanCommandPool>	m_commandPools2;

		//std::vector<vk::UniqueCommandPool>	m_commandPools;				// Available pools to allocate command buffers from
		//std::vector<vk::CommandBuffer>		m_availableCommandBuffers;	// Available command buffers
		//std::vector<vk::CommandBuffer>		m_recordedCommandBuffers;	// Command buffers that have been used

		vk::CommandPoolCreateInfo		m_poolCreateInfo;	// keep it for when we need to reallocate a new pool.
		vk::CommandBufferAllocateInfo	m_bufferCreateInfo;	// keep it for when we need to reallocate a new pool.
		uint32_t						m_reservedBuffersPerPool = DEFAULT_BUFFERS_PER_POOL; // a safe number to begin with ?


	};
}


#endif // MOE_VULKAN
