#ifdef MOE_VULKAN

#include "VulkanLightSystem.h"

namespace moe
{
	void VulkanLightSystem::InitializeGPUResources()
	{
		static const auto DEFAULT_LIGHT_NUMBER = 10; // TODO : set to 1 once we have proper reallocation systems in place...

		// Create the lights buffers
		auto LightInitializerFun = [&](auto& lightData)
		{
			using LightType = std::decay_t<decltype(*lightData.DeviceMemoryMap)>; // use decay to remove the reference

			lightData.HostMemory.Reserve(DEFAULT_LIGHT_NUMBER);

			// Create a host-visible buffer that will be updateable without having to flush (coherent)
			lightData.DeviceBuffer = VulkanBuffer::NewUniformBuffer(Device(), sizeof(LightType) * lightData.HostMemory.GetCapacity() * m_frameCount,
			nullptr, VulkanBuffer::NoTransfer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

			// Leave the buffer mapped forever.
			lightData.DeviceMemoryMap = (LightType*)lightData.DeviceBuffer.Map(Device());
		};

		LightInitializerFun(m_directionalLights);
		LightInitializerFun(m_pointLights);
		LightInitializerFun(m_spotLights);


		// Now allocate the descriptor set of the light system.
		const auto stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
		std::array<vk::DescriptorSetLayoutBinding, (size_t)LightType::_COUNT_> lightSystemBindings;
		for (auto iBinding = 0; iBinding < (size_t)LightType::_COUNT_; ++iBinding)
		{
			lightSystemBindings[iBinding].binding = iBinding;
			lightSystemBindings[iBinding].stageFlags = stageFlags;
		}

		vk::DescriptorSetLayoutCreateInfo layoutCreateInfo{
			vk::DescriptorSetLayoutCreateFlags(),
			(uint32_t) lightSystemBindings.size(),
			lightSystemBindings.data()
		};

		m_setAllocator.InitializePool(Device(), layoutCreateInfo, DEFAULT_LIGHT_NUMBER * m_frameCount);
	}


	void VulkanLightSystem::Initialize(MyVkDevice& device, uint32_t frameCount)
	{
		m_device = &device;
		m_frameCount = frameCount;
	}
}


#endif // MOE_VULKAN
