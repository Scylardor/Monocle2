#ifdef MOE_VULKAN

#include "VulkanCommandPool.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"

namespace moe
{
	VulkanCommandPool::VulkanCommandPool(const MyVkDevice& device, const vk::CommandPoolCreateInfo& poolCreateInfo,
		vk::CommandBufferAllocateInfo& bufferCreateInfo)
	{
		m_pool = device->createCommandPoolUnique(poolCreateInfo);
		MOE_ASSERT(m_pool);

		// patch the command buffer allocate with our pool handle
		bufferCreateInfo.commandPool = m_pool.get();

		m_availableCommandBuffers.resize(bufferCreateInfo.commandBufferCount);
		MOE_VK_CHECK(device->allocateCommandBuffers(&bufferCreateInfo, m_availableCommandBuffers.data() + m_availableCommandBuffers.size()));

		m_recordedCommandBuffers.reserve(bufferCreateInfo.commandBufferCount);
	}


	bool VulkanCommandPool::Initialize(const MyVkDevice& device, uint32_t /*wantedCapacity*/, vk::CommandPoolCreateFlagBits flags)
	{
		MOE_ASSERT(device.HasRequiredGraphicsQueueFamilies());

		vk::CommandPoolCreateInfo createInfo{ flags, device.GraphicsQueueIdx() };
		m_pool = device->createCommandPoolUnique(createInfo);
		MOE_ASSERT(m_pool);

		return true;
	}


	void VulkanCommandPool::Reset(const MyVkDevice& device)
	{
		// leave flag bits to 0 to never release resources to the GPU (it's going to be reused)
		device->resetCommandPool(m_pool.get(), vk::CommandPoolResetFlagBits());

		// The pool has been reinitialized : put all the allocated command buffers back into the available list
		m_availableCommandBuffers.insert(m_availableCommandBuffers.end(), m_recordedCommandBuffers.begin(), m_recordedCommandBuffers.end());
		m_recordedCommandBuffers.clear();
	}

	// TODO: a bit dangerous, FreeBuffers could never be called ! Better to put it in the dtor ? (but we need the device... deleter?)
	void VulkanCommandPool::FreeBuffers(const MyVkDevice& device)
	{
		Reset(device);
		device->freeCommandBuffers(m_pool.get(), (uint32_t) m_availableCommandBuffers.size(), m_availableCommandBuffers.data());
	}


	std::optional<vk::CommandBuffer> VulkanCommandPool::TryGrabCommandBuffer()
	{
		if (m_availableCommandBuffers.empty())
			return std::nullopt;

		auto vkCmdBuf = m_availableCommandBuffers.back();
		m_availableCommandBuffers.pop_back();
		m_recordedCommandBuffers.push_back(vkCmdBuf);
		return vkCmdBuf;
	}
}


#endif // MOE_VULKAN
