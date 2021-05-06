
#ifdef MOE_VULKAN

#include "VulkanBuffer.h"
#include "Graphics/Vulkan/Device/VulkanDevice.h"

#include "Core/Misc/Types.h"

namespace moe
{

	VulkanBuffer::VulkanBuffer(BufferHandles buffer, BufferHandles staging, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memoryProperties, vk::DeviceSize size, vk::DeviceSize offset) :
		m_buffer(std::move(buffer)), m_staging(std::move(staging)), m_usage(usage), m_memoryProperties(memoryProperties), m_size(size)
	{
		m_descriptor.buffer = buffer.Buffer;
		m_descriptor.range = size;
		m_descriptor.offset = offset;
	}


	void* VulkanBuffer::Map(MyVkDevice& device, vk::DeviceSize offset, vk::DeviceSize range)
	{
		MOE_ASSERT(m_staging.MemoryBlock || m_memoryProperties & vk::MemoryPropertyFlagBits::eHostVisible);

		// If there is a staging buffer, map the staging buffer. Else, try to map the actual buffer
		VulkanMemoryBlock& mappedBlock = (m_staging.MemoryBlock ? m_staging.MemoryBlock : m_buffer.MemoryBlock);

		auto mappedRange = (range == VK_WHOLE_SIZE ? m_size : range);
		void* mappedMemory = device.MemoryAllocator().MapMemory(mappedBlock, mappedRange, offset);

		m_mapping.size = m_size;
		m_mapping.offset = offset;
		m_mapping.memory = mappedBlock.Memory;

		return mappedMemory;
	}

	void VulkanBuffer::Unmap(MyVkDevice& device, StagingTransfer transferMode)
	{
		if (!m_staging.MemoryBlock && !(m_memoryProperties & vk::MemoryPropertyFlagBits::eHostCoherent))
		{
			MOE_VK_CHECK(device->flushMappedMemoryRanges(1, &m_mapping));
		}

		VulkanMemoryBlock& block = (m_staging.MemoryBlock ? m_staging.MemoryBlock : m_buffer.MemoryBlock);
		device.MemoryAllocator().UnmapMemory(block);

		// Following doesn't apply if we dont have staging memory
		if (m_staging.MemoryBlock)
		{
			switch (transferMode)
			{
			case NoTransfer:
				break;
			case ImmediateTransfer:
				Copy(device, m_staging.Buffer, m_buffer.Buffer, m_mapping.size);
				DeleteStagingBuffer(device);
				break;
			default:
				MOE_ASSERT(false); // Not implemented !
			}
		}

		m_mapping = vk::MappedMemoryRange{};
	}


	void VulkanBuffer::Free(MyVkDevice& device)
	{
		device.BufferAllocator().ReleaseBufferHandles(m_buffer);
		device.BufferAllocator().ReleaseBufferHandles(m_staging);
	}


	void VulkanBuffer::DeleteStagingBuffer(MyVkDevice& device)
	{
		if (m_staging.Buffer && m_staging.MemoryBlock)
		{
			device.BufferAllocator().ReleaseBufferHandles(m_staging);
		}
	}


	VulkanBuffer VulkanBuffer::NewBuffer(MyVkDevice& device, VkDeviceSize bufferSize, const void* bufferData, vk::BufferUsageFlagBits specificUsageFlags, StagingTransfer transferMode, vk::MemoryPropertyFlags memoryProperties)
	{
		VulkanBuffer buffer = device.BufferAllocator().Create(bufferSize, specificUsageFlags, memoryProperties);

		MOE_ASSERT((bufferSize != 0 && bufferData != nullptr) || (bufferSize == 0 && bufferData == nullptr));
		if (bufferData != nullptr)
		{
			auto* ptr = buffer.Map(device);
			memcpy(ptr, bufferData, bufferSize);
			buffer.Unmap(device, transferMode);
		}



		return buffer;
	}


	VulkanBuffer VulkanBuffer::NewVertexBuffer(MyVkDevice& device, VkDeviceSize bufferSize, const void* bufferData, StagingTransfer transferMode, vk::MemoryPropertyFlags memoryProperties)
	{
		VulkanBuffer vertexBuffer = NewBuffer(device, bufferSize, bufferData, vk::BufferUsageFlagBits::eVertexBuffer, transferMode, memoryProperties);
		return vertexBuffer;
	}

	VulkanBuffer VulkanBuffer::NewIndexBuffer(MyVkDevice& device, VkDeviceSize bufferSize, const void* bufferData, StagingTransfer transferMode, vk::MemoryPropertyFlags memoryProperties)
	{
		VulkanBuffer indexBuffer = NewBuffer(device, bufferSize, bufferData, vk::BufferUsageFlagBits::eIndexBuffer, transferMode, memoryProperties);
		return indexBuffer;
	}

	VulkanBuffer VulkanBuffer::NewUniformBuffer(MyVkDevice& device, VkDeviceSize bufferSize, const void* bufferData, StagingTransfer transferMode, vk::MemoryPropertyFlags memoryProperties)
	{
		MOE_ASSERT(bufferSize <= device.Properties().limits.maxUniformBufferRange); // or this allocation is going to fail !
		VulkanBuffer uniformBuffer = NewBuffer(device, bufferSize, bufferData, vk::BufferUsageFlagBits::eUniformBuffer, transferMode, memoryProperties);
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

}


#endif // MOE_VULKAN
