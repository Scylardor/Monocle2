#include "VulkanCommandPipeline.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"


moe::VulkanCommandPipeline::~VulkanCommandPipeline()
{
	// For command buffers, I prefer to delete them myself at pipeline destruction time.
	for (auto& cmdPool : m_commandPools2)
	{
		cmdPool.FreeBuffers(Device());
	}
}


bool moe::VulkanCommandPipeline::Initialize(const MyVkDevice& device, vk::CommandBufferLevel cbLevel,
											vk::CommandPoolCreateFlagBits flags,
											uint32_t reservedPools, uint32_t reservedBuffersPerPool)
{
	MOE_ASSERT(m_commandPools2.empty() && m_device == nullptr);
	m_device = &device;

	m_reservedBuffersPerPool = reservedBuffersPerPool;

	MOE_ASSERT(device.HasRequiredGraphicsQueueFamilies());
	m_poolCreateInfo = vk::CommandPoolCreateInfo{ flags, device.GraphicsQueueIdx() };

	m_bufferCreateInfo = vk::CommandBufferAllocateInfo{ vk::CommandPool(), cbLevel, m_reservedBuffersPerPool };

	m_commandPools2.reserve(reservedPools);

	for (auto iPool = 0u; iPool < reservedPools; iPool++)
	{
		m_commandPools2.emplace_back(device, m_poolCreateInfo, m_bufferCreateInfo, m_reservedBuffersPerPool);
	}

	return true;
}


void moe::VulkanCommandPipeline::Reset(const MyVkDevice& device)
{
	for (auto& cmdPool : m_commandPools2)
	{
		cmdPool.Reset(device);
	}
}


vk::CommandBuffer moe::VulkanCommandPipeline::GrabCommandBuffer(const MyVkDevice& device)
{
	for (auto& cmdPool : m_commandPools2)
	{
		auto cmdBuf = cmdPool.TryGrabCommandBuffer();
		if (cmdBuf.has_value())
			return *cmdBuf;
	}

	// no more buffers: all our pools have been exhausted ! We need to create a new pool.
	auto& newPool = m_commandPools2.emplace_back(device, m_poolCreateInfo, m_bufferCreateInfo, m_reservedBuffersPerPool);
	auto cmdBuf = newPool.TryGrabCommandBuffer();
	MOE_ASSERT(cmdBuf.has_value());

	return *cmdBuf;
}

