#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

#include <vector>

#include <optional>

namespace moe
{
	class MyVkDevice;

	class VulkanCommandPool
	{
	public:

		VulkanCommandPool(const MyVkDevice& device, const vk::CommandPoolCreateInfo& poolCreateInfo,
							vk::CommandBufferAllocateInfo& bufferCreateInfo);


		bool	Initialize(const MyVkDevice& device, uint32_t wantedCapacity, vk::CommandPoolCreateFlagBits flags = vk::CommandPoolCreateFlagBits());


		void	Reset(const MyVkDevice& device);


		void	FreeBuffers(const MyVkDevice& device);

		std::optional<vk::CommandBuffer>	TryGrabCommandBuffer();

	protected:

	private:

		vk::UniqueCommandPool			m_pool{};
		std::vector<vk::CommandBuffer>	m_availableCommandBuffers;	// Available command buffers
		std::vector<vk::CommandBuffer>	m_recordedCommandBuffers;	// Command buffers that have been used
	};
}


#endif // MOE_VULKAN
