#include "VulkanCommandPipeline.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"


moe::VulkanCommandPipeline::~VulkanCommandPipeline()
{
	// For command buffers, I prefer to delete them myself at pipeline destruction time.
	for (auto& cmdPool : m_commandPools)
	{
		cmdPool.FreeBuffers(Device());
	}
}


bool moe::VulkanCommandPipeline::Initialize(const MyVkDevice& device, vk::CommandBufferLevel cbLevel,
											vk::CommandPoolCreateFlagBits flags,
											uint32_t reservedPools, uint32_t reservedBuffersPerPool)
{
	MOE_ASSERT(m_commandPools.empty() && m_device == nullptr);
	m_device = &device;

	MOE_ASSERT(device.HasRequiredGraphicsQueueFamilies());
	m_poolCreateInfo = vk::CommandPoolCreateInfo{ flags, device.GraphicsQueueIdx() };

	m_bufferCreateInfo = vk::CommandBufferAllocateInfo{ vk::CommandPool(), cbLevel, reservedBuffersPerPool };

	m_commandPools.reserve(reservedPools);

	for (auto iPool = 0u; iPool < reservedPools; iPool++)
	{
		m_commandPools.emplace_back(device, m_poolCreateInfo, m_bufferCreateInfo);
	}

	return true;
}


void moe::VulkanCommandPipeline::Reset(const MyVkDevice& device)
{
	for (auto& cmdPool : m_commandPools)
	{
		cmdPool.Reset(device);
	}
}


vk::CommandBuffer moe::VulkanCommandPipeline::GrabCommandBuffer(const MyVkDevice& device)
{
	for (auto& cmdPool : m_commandPools)
	{
		auto cmdBuf = cmdPool.TryGrabCommandBuffer();
		if (cmdBuf.has_value())
			return *cmdBuf;
	}

	// no more buffers: all our pools have been exhausted ! We need to create a new pool.
	auto& newPool = m_commandPools.emplace_back(device, m_poolCreateInfo, m_bufferCreateInfo);
	auto cmdBuf = newPool.TryGrabCommandBuffer();
	MOE_ASSERT(cmdBuf.has_value());

	return *cmdBuf;
}

