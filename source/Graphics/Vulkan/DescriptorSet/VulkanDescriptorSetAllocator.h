#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

#include "Graphics/Vulkan/Swapchain/VulkanSwapchain.h"

#include <stack>

namespace moe
{
	class MyVkDevice;

	class VulkanDescriptorSetAllocator
	{
	public:

		~VulkanDescriptorSetAllocator();

		void				InitializePool(MyVkDevice& device, const vk::DescriptorSetLayoutCreateInfo& layoutCreateInfo, uint32_t maxSetsPerPool = 1);

		std::vector<vk::DescriptorSet>	Allocate(uint32_t nbWantedSets);

		void				Free(vk::DescriptorSet freedSet);

	protected:

	private:

		MOE_VK_DEVICE_GETTER()

		void	AllocateNewDescriptorPool();

		void	InitializeDescriptorSetLayout(const vk::DescriptorSetLayoutCreateInfo& layoutCreateInfo);


		MyVkDevice*						m_device{ nullptr };

		std::vector<vk::DescriptorPool>	m_pools{};

		std::vector<vk::DescriptorSet>	m_availableDescriptorSets;

		vk::DescriptorSetLayout			m_allocatedLayout{};


		VkDescriptorSetLayoutCreateInfo	m_layoutCreateInfo{};

		uint32_t						m_maxSetsPerPool = 1;
		uint32_t						m_frameCount = VulkanSwapchain::GetMaxFramesInFlight();

		std::vector<vk::DescriptorPoolSize>	m_poolSizes{};
	};
}


#endif // MOE_VULKAN
