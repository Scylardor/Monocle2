
#ifdef MOE_VULKAN

#include "VulkanBuffer.h"
#include "Graphics/Vulkan/Device/VulkanDevice.h"
#include "Graphics/Vulkan/CommandPool/VulkanCommandPool.h"

namespace moe
{
	VulkanBuffer VulkanBuffer::NewDeviceBuffer(const MyVkDevice& /*device*/)
	{
		return VulkanBuffer();
	}


	VulkanBuffer VulkanBuffer::NewStagingBuffer(const MyVkDevice& device, VkDeviceSize bufferSize)
	{
		VulkanBuffer stagingBuffer = CreateBuffer(device, bufferSize,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		return stagingBuffer;
	}


	VulkanBuffer VulkanBuffer::NewVertexBuffer(const MyVkDevice& device, VkDeviceSize bufferSize, byte_t* bufferData)
	{
		// First create a staging buffer
		VulkanBuffer staging = NewStagingBuffer(device, bufferSize);

		// Then upload the data in the staging buffer
		void* mappedBuf = device->mapMemory(staging.Memory(), 0, bufferSize, vk::MemoryMapFlags());
		MOE_ASSERT(mappedBuf != nullptr);
		memcpy(mappedBuf, bufferData, bufferSize);
		device->unmapMemory(staging.Memory());

		// Now create our "final" device-local buffer
		VulkanBuffer vertexBuffer = CreateBuffer(device, bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);

		// Schedule the copy operation from the staging buffer to the device-local buffer




		return vertexBuffer;
	}


	void VulkanBuffer::Copy(const MyVkDevice& device, vk::Buffer from, vk::Buffer to, VkDeviceSize size)
	{
		// TODO: this will have to be refactored in a BufferFactory and a TransientCommandPool.

		// Use a transient command pool with one-time submit command buffers to inform the driver this is going to be short-lived
		vk::CommandPoolCreateInfo poolCreateInfo{ vk::CommandPoolCreateFlagBits::eTransient, device.GraphicsQueueIdx() };
		vk::CommandBufferAllocateInfo cbCreateInfo{ vk::CommandPool(), vk::CommandBufferLevel::ePrimary, 1 };
		VulkanCommandPool tmpPool{device, poolCreateInfo, cbCreateInfo };
		tmpPool.Initialize(device, 1, vk::CommandPoolCreateFlagBits::eTransient);

		auto tmpCmdBuffer = tmpPool.TryGrabCommandBuffer();
		MOE_ASSERT(tmpCmdBuffer.has_value());

		(*tmpCmdBuffer).begin(vk::CommandBufferBeginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

		vk::BufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = size;
		(*tmpCmdBuffer).copyBuffer(from, to, 1, &copyRegion);

		(*tmpCmdBuffer).end();

		vk::SubmitInfo submitInfo{};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &tmpCmdBuffer.value();

		// Now create a fence to wait on
		vk::FenceCreateInfo fenceCreateInfo{
			vk::FenceCreateFlagBits()
		};

		vk::UniqueFence submitFence = device->createFenceUnique(fenceCreateInfo);

		device.GraphicsQueue().submit(1, &submitInfo, submitFence.get());

		// Now wait for the operation to complete
		static const bool WAIT_ALL = true;
		static const auto NO_TIMEOUT = UINT64_MAX;
		device->waitForFences(1, &submitFence.get(), WAIT_ALL, NO_TIMEOUT);

		// The operation is over !
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
