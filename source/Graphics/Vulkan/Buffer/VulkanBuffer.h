#pragma once
#include "Graphics/Vulkan/Allocators/MemoryAllocator.h"
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

namespace moe
{
	class MyVkDevice;

	struct BufferHandles
	{
		vk::UniqueBuffer		Buffer{};
		VulkanMemoryBlock		MemoryBlock{};
	};



	class VulkanBuffer
	{
	public:

		VulkanBuffer() = default;

		VulkanBuffer(BufferHandles buffer, BufferHandles staging, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memoryProperties, vk::DeviceSize size, vk::DeviceSize offset = 0);

		void*		Map(MyVkDevice& device, vk::DeviceSize offset = 0, vk::DeviceSize range = VK_WHOLE_SIZE);

		enum StagingTransfer
		{
			NoTransfer,
			ImmediateTransfer,	// transfer immediately using a temporary one-time submit command buffer
			BatchedTransfer		// TODO: wait until start of frame to transfer
		};
		void		Unmap(MyVkDevice& device, StagingTransfer transferMode = StagingTransfer::NoTransfer);


		void	DeleteStagingBuffer(MyVkDevice& device);


		[[nodiscard]] static VulkanBuffer	NewBuffer(MyVkDevice& device, VkDeviceSize bufferSize, const void* bufferData, vk::BufferUsageFlagBits specificUsageFlags, StagingTransfer transferMode, vk::MemoryPropertyFlags memoryProperties);

		[[nodiscard]] static VulkanBuffer	NewVertexBuffer(MyVkDevice& device, VkDeviceSize bufferSize, const void* bufferData, StagingTransfer transferMode = ImmediateTransfer, vk::MemoryPropertyFlags memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal);

		[[nodiscard]] static VulkanBuffer	NewIndexBuffer(MyVkDevice& device, VkDeviceSize bufferSize, const void* bufferData, StagingTransfer transferMode = ImmediateTransfer, vk::MemoryPropertyFlags memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal);

		[[nodiscard]] static VulkanBuffer	NewUniformBuffer(MyVkDevice& device, VkDeviceSize bufferSize, const void* bufferData, StagingTransfer transferMode = ImmediateTransfer, vk::MemoryPropertyFlags memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal);

		[[nodiscard]] static vk::DeviceSize	Align(vk::DeviceSize size, vk::DeviceSize alignment)
		{
			if (alignment > 0)
				size = (size + alignment - 1) & ~(alignment - 1);

			return size;
		}

		static void		Copy(const MyVkDevice& device, vk::Buffer from, vk::Buffer to, VkDeviceSize size);

		vk::DeviceMemory	Memory() const
		{
			return m_buffer.MemoryBlock.Memory.get();
		}

		vk::Buffer			Handle() const
		{
			return m_buffer.Buffer.get();
		}

		operator vk::Buffer() const
		{
			MOE_ASSERT(m_buffer.Buffer);
			return m_buffer.Buffer.get();
		}

	protected:

	private:


		BufferHandles	m_buffer{};
		BufferHandles	m_staging{};

		vk::BufferUsageFlags		m_usage{};
		vk::MemoryPropertyFlags		m_memoryProperties{};
		vk::DeviceSize				m_size{};

		vk::DescriptorBufferInfo	m_descriptor{};

		vk::MappedMemoryRange		m_mapping{};

	};
}


#endif // MOE_VULKAN
