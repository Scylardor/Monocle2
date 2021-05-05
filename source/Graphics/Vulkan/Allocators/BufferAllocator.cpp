#ifdef MOE_VULKAN

#include "BufferAllocator.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"

namespace moe
{
	// For now, this function naively allocates a new buffer each time and does not attempt to reuse freed memory.
	VulkanBuffer VulkanBufferAllocator::Create(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memoryProperties)
	{
		// You always have to make sure the allocated size is aligned to nonCoherentAtomSize.
		vk::DeviceSize alignment = m_device.Properties().limits.nonCoherentAtomSize;
		size += (alignment - (size % alignment));

		const bool needsStagingBuffer = (memoryProperties & vk::MemoryPropertyFlagBits::eDeviceLocal) == vk::MemoryPropertyFlagBits::eDeviceLocal;

		// First allocate the staging buffer if necessary
		BufferHandles staging{};

		if (needsStagingBuffer)
		{
			usage |= vk::BufferUsageFlagBits::eTransferDst; // just in case

			staging = CreateStagingBuffer(size);
		}

		// Then create the "actual" buffer
		BufferHandles buffer = CreateBufferHandles(size, usage, memoryProperties);

		return VulkanBuffer{std::move(buffer), std::move(staging), usage, memoryProperties, size, 0};
	}


	BufferHandles VulkanBufferAllocator::CreateStagingBuffer(vk::DeviceSize size)
	{
		BufferHandles staging{};

		vk::MemoryPropertyFlags stagingProps = (vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		vk::BufferUsageFlags stagingUsage = vk::BufferUsageFlagBits::eTransferSrc;

		staging = CreateBufferHandles(size, stagingUsage, stagingProps);

		return staging;
	}


	void VulkanBufferAllocator::ReleaseBufferHandles(BufferHandles& handles)
	{
		if (handles.Buffer)
		{
			m_device->destroyBuffer(handles.Buffer);
			handles.Buffer = vk::Buffer{};
		}
		if (handles.MemoryBlock)
		{
			m_device.MemoryAllocator().FreeBufferDeviceMemory(handles.MemoryBlock);
		}
	}


	BufferHandles VulkanBufferAllocator::CreateBufferHandles(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memoryProperties) const
	{
		vk::BufferCreateInfo bufferInfo{};
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = vk::SharingMode::eExclusive; // assume graphics and transfer queue families are the same, probably... // TODO: check ?

		vk::Buffer buff = m_device->createBuffer(bufferInfo);
		MOE_ASSERT(buff);

		VulkanMemoryBlock memBlock = m_device.MemoryAllocator().AllocateBufferDeviceMemory(buff, memoryProperties);
		MOE_ASSERT(memBlock);

		m_device->bindBufferMemory(buff, memBlock.Memory, 0);

		return BufferHandles{ buff, std::move(memBlock) };
	}
}


#endif // MOE_VULKAN
