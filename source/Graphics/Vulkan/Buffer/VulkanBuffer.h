#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

namespace moe
{
	class MyVkDevice;

	class VulkanBuffer
	{
	public:

		static VulkanBuffer	NewDeviceBuffer(const MyVkDevice& device, VkDeviceSize bufferSize, const void* bufferData, vk::BufferUsageFlagBits specificUsageFlags);

		static VulkanBuffer	NewStagingBuffer(const MyVkDevice& device, VkDeviceSize bufferSize);

		static VulkanBuffer	NewVertexBuffer(const MyVkDevice& device, VkDeviceSize bufferSize, const void* bufferData);

		static VulkanBuffer	NewIndexBuffer(const MyVkDevice& device, VkDeviceSize bufferSize, const void* bufferData);

		static VulkanBuffer	NewUniformBuffer(const MyVkDevice& device, VkDeviceSize bufferSize, const void* bufferData);


		static void		Copy(const MyVkDevice& device, vk::Buffer from, vk::Buffer to, VkDeviceSize size);

		vk::DeviceMemory	Memory() const
		{
			return m_bufferMemory.get();
		}

		operator vk::Buffer() const
		{
			MOE_ASSERT(m_buffer);
			return m_buffer.get();
		}

	protected:

	private:

		static VulkanBuffer	CreateBuffer(const MyVkDevice& device, VkDeviceSize bufferSize, vk::BufferUsageFlags bufferUsage, vk::MemoryPropertyFlags memoryProperties);


		vk::UniqueBuffer		m_buffer{};
		vk::UniqueDeviceMemory	m_bufferMemory{};
	};
}


#endif // MOE_VULKAN
