#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

namespace moe
{
	struct VulkanMemoryBlock;
	class MyVkDevice;

	struct BufferMap
	{
	public:

		BufferMap() = default;

		BufferMap(vk::Device dev, vk::DeviceMemory mem, vk::DeviceSize offset, vk::DeviceSize range) :
			m_Device(dev), m_Mem(mem)
		{
			m_Ptr = m_Device.mapMemory(m_Mem, offset, range, vk::MemoryMapFlags());
		}

		~BufferMap()
		{
			if (m_Device && m_Mem)
				m_Device.unmapMemory(m_Mem);
		}

		template <typename TMap>
		[[nodiscard]] TMap* As()
		{
			return static_cast<TMap*>(Pointer());
		}

		template <typename TMap>
		[[nodiscard]] const TMap* As() const
		{
			return static_cast<const TMap*>(Pointer());
		}

		[[nodiscard]] const void* Pointer() const
		{
			return m_Ptr;
		}

		void*	Pointer()
		{
			return m_Ptr;
		}


		void**	MappedRange()
		{
			return &m_Ptr;
		}


	private:
		void* m_Ptr = nullptr;
		vk::Device			m_Device{};
		vk::DeviceMemory	m_Mem{};

	};


	struct VulkanMemoryBlock
	{
		vk::DeviceMemory		Memory{};
		vk::DeviceSize			Offset{0};
	//	uint32_t				Heap{VK_MAX_MEMORY_HEAPS};
		bool					IsMapped = false;

		operator bool() const
		{
			return Memory;
		}
	};


	class VulkanMemoryAllocator
	{
	public:
		VulkanMemoryAllocator() = default;

		VulkanMemoryAllocator(MyVkDevice& device);


		VulkanMemoryBlock	AllocateBufferDeviceMemory(vk::Buffer buffer,
		                                                  vk::MemoryPropertyFlags memPropertiesFlags) const;

		VulkanMemoryBlock	AllocateTextureDeviceMemory(vk::Image image, vk::MemoryPropertyFlags memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal) const;

		void				FreeBufferDeviceMemory(VulkanMemoryBlock& block);


		void*		MapMemory(VulkanMemoryBlock& block, vk::DeviceSize size, vk::DeviceSize offset = 0, vk::MemoryMapFlags flags = {});

		void		UnmapMemory(VulkanMemoryBlock& mapping);

	protected:

	private:

		uint32_t	FindSuitableMemoryTypeIndex(uint32_t typeFilterBits, vk::MemoryPropertyFlags memoryProperties) const;

		MyVkDevice*	m_device;

		vk::PhysicalDeviceMemoryProperties	m_memProps;
	};
}


#endif // MOE_VULKAN
