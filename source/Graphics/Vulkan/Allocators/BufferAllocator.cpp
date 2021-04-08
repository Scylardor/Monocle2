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

			vk::MemoryPropertyFlags stagingProps = (vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
			vk::BufferUsageFlags stagingUsage = vk::BufferUsageFlagBits::eTransferSrc;

			staging = CreateBufferHandles(size, stagingUsage, stagingProps);
		}

		// Then create the "actual" buffer
		BufferHandles buffer = CreateBufferHandles(size, usage, memoryProperties);

		return VulkanBuffer{std::move(buffer), std::move(staging), usage, memoryProperties, size, 0};
	}


	void VulkanBufferAllocator::ReleaseBufferHandles(BufferHandles& handles)
	{
		if (handles.Buffer)
		{
			handles.Buffer.reset();
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

		vk::UniqueBuffer buff = m_device->createBufferUnique(bufferInfo);
		MOE_ASSERT(buff);

		VulkanMemoryBlock memBlock = m_device.MemoryAllocator().AllocateBufferDeviceMemory(buff.get(), memoryProperties);
		MOE_ASSERT(memBlock);

		m_device->bindBufferMemory(buff.get(), memBlock.Memory.get(), 0);

		return BufferHandles{ std::move(buff), std::move(memBlock) };
	}
}


#endif // MOE_VULKAN
