#ifdef MOE_VULKAN

#include "MemoryAllocator.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"

namespace moe
{
	VulkanMemoryAllocator::VulkanMemoryAllocator(MyVkDevice& device):
		m_device(device)
	{
		m_memProps = m_device.PhysicalDevice().getMemoryProperties();
	}

	// For now, this function naively allocates a new device memory each time and does not attempt to reuse freed memory.
	VulkanMemoryBlock VulkanMemoryAllocator::AllocateBufferDeviceMemory(vk::Buffer buffer, vk::MemoryPropertyFlags memPropertiesFlags) const
	{
		// First retrieve the memory requirements for this buffer and properties
		const auto memRequirements = m_device->getBufferMemoryRequirements(buffer);

		// Now we need to find the right memory type index for the returned requirements...
		uint32_t memTypeIndex = UINT32_MAX;
		for (uint32_t iMemType = 0; iMemType < m_memProps.memoryTypeCount; iMemType++)
		{
			if (memRequirements.memoryTypeBits & (1 << iMemType) && (m_memProps.memoryTypes[iMemType].propertyFlags & memPropertiesFlags) == memPropertiesFlags)
			{
				memTypeIndex = iMemType;
				break;
			}
		}

		MOE_ASSERT(memTypeIndex != UINT32_MAX);

		const vk::MemoryAllocateInfo allocInfo{
			memRequirements.size,
			memTypeIndex
		};

		auto bufferMemory = m_device->allocateMemoryUnique(allocInfo);
		MOE_ASSERT(bufferMemory);

		return VulkanMemoryBlock{ std::move(bufferMemory), 0 };
	}


	void VulkanMemoryAllocator::FreeBufferDeviceMemory(VulkanMemoryBlock& block)
	{
		if (block.Memory)
		{
			block.Memory.reset();
		}
	}


	void* VulkanMemoryAllocator::MapMemory(VulkanMemoryBlock& block, vk::DeviceSize size, vk::DeviceSize offset, vk::MemoryMapFlags flags)
	{
		MOE_ASSERT(block.IsMapped == false); // It is an application error to call vkMapMemory on a memory object that is already host mapped.
		block.IsMapped = true;

		void* mapping;

		m_device->mapMemory(block.Memory.get(), offset, size, flags, &mapping);

		return mapping;
	}


	void VulkanMemoryAllocator::UnmapMemory(VulkanMemoryBlock& block)
	{
		MOE_ASSERT(block.IsMapped);
		block.IsMapped = false;

		m_device->unmapMemory(block.Memory.get());
	}
}


#endif // MOE_VULKAN
