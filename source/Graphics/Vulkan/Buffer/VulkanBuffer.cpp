
#ifdef MOE_VULKAN

#include "VulkanBuffer.h"
#include "Graphics/Vulkan/Device/VulkanDevice.h"
#include "Graphics/Vulkan/CommandPool/VulkanCommandPool.h"

namespace moe
{
	VulkanBuffer VulkanBuffer::NewDeviceBuffer(const MyVkDevice& device, VkDeviceSize bufferSize, const void* bufferData, vk::BufferUsageFlagBits specificUsageFlags)
	{
		// First create a staging buffer
		VulkanBuffer staging = NewStagingBuffer(device, bufferSize);

		// Then upload the data in the staging buffer
		void* mappedBuf = device->mapMemory(staging.Memory(), 0, bufferSize, vk::MemoryMapFlags());
		MOE_ASSERT(mappedBuf != nullptr);
		memcpy(mappedBuf, bufferData, bufferSize);
		device->unmapMemory(staging.Memory());

		// Now create our "final" device-local buffer
		VulkanBuffer deviceBuffer = CreateBuffer(device, bufferSize, vk::BufferUsageFlagBits::eTransferDst | specificUsageFlags, vk::MemoryPropertyFlagBits::eDeviceLocal);

		// Schedule the copy operation from the staging buffer to the device-local buffer
		Copy(device, staging, deviceBuffer, bufferSize);

		return deviceBuffer;
	}


	VulkanBuffer VulkanBuffer::NewStagingBuffer(const MyVkDevice& device, VkDeviceSize bufferSize)
	{
		VulkanBuffer stagingBuffer = CreateBuffer(device, bufferSize,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		return stagingBuffer;
	}


	VulkanBuffer VulkanBuffer::NewVertexBuffer(const MyVkDevice& device, VkDeviceSize bufferSize, const byte_t* bufferData)
	{
		VulkanBuffer vertexBuffer = NewDeviceBuffer(device, bufferSize, bufferData, vk::BufferUsageFlagBits::eVertexBuffer);
		return vertexBuffer;
	}

	VulkanBuffer VulkanBuffer::NewIndexBuffer(const MyVkDevice& device, VkDeviceSize bufferSize, const byte_t* bufferData)
	{
		VulkanBuffer indexBuffer = NewDeviceBuffer(device, bufferSize, bufferData, vk::BufferUsageFlagBits::eIndexBuffer);
		return indexBuffer;
	}

	VulkanBuffer VulkanBuffer::NewUniformBuffer(const MyVkDevice& device, VkDeviceSize bufferSize, const byte_t* bufferData)
	{
		VulkanBuffer uniformBuffer = NewDeviceBuffer(device, bufferSize, bufferData, vk::BufferUsageFlagBits::eUniformBuffer);
		return uniformBuffer;
	}


	void VulkanBuffer::Copy(const MyVkDevice& device, vk::Buffer from, vk::Buffer to, VkDeviceSize size)
	{
		// TODO: this will have to be refactored in a BufferFactory and a TransientCommandPool.
		device.ImmediateCommandSubmit([=](vk::CommandBuffer recordingBuffer)
			{
				vk::BufferCopy copyRegion{};
				copyRegion.srcOffset = 0; // Optional
				copyRegion.dstOffset = 0; // Optional
				copyRegion.size = size;
				recordingBuffer.copyBuffer(from, to, 1, &copyRegion);
			});

	}


	VulkanBuffer VulkanBuffer::CreateBuffer(const MyVkDevice& device, VkDeviceSize bufferSize, vk::BufferUsageFlags bufferUsage, vk::MemoryPropertyFlags memoryProperties)
	{
		vk::BufferCreateInfo bufferInfo{};
		bufferInfo.size = bufferSize;
		bufferInfo.usage = bufferUsage;
		bufferInfo.sharingMode = vk::SharingMode::eExclusive; // assume graphics and transfer queue families are the same, probably... // TODO: check ?

		VulkanBuffer newBuffer;
		newBuffer.m_buffer = device->createBufferUnique(bufferInfo);
		newBuffer.m_bufferMemory = device.AllocateBufferDeviceMemory(newBuffer, memoryProperties);

		device->bindBufferMemory(newBuffer.m_buffer.get(), newBuffer.m_bufferMemory.get(), 0);

		return newBuffer;
	}
}


#endif // MOE_VULKAN
