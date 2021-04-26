#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

#include "Graphics/Vulkan/Buffer/VulkanBuffer.h"

namespace moe
{
	class MyVkDevice;

	class VulkanBufferAllocator
	{
	public:

		VulkanBufferAllocator(MyVkDevice& device) :
			m_device(device)
		{}

		VulkanBuffer	Create(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memoryProperties);

		BufferHandles	CreateStagingBuffer(vk::DeviceSize size);

		void			ReleaseBufferHandles(BufferHandles& handles);


	protected:

	private:

		BufferHandles	CreateBufferHandles(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memoryProperties) const;


		MyVkDevice&	m_device;
	};
}


#endif // MOE_VULKAN
