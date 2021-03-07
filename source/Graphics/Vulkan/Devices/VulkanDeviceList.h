
#ifdef MOE_VULKAN

#pragma once

#include <vulkan/vulkan.hpp>

namespace moe
{
	// TODO: Should be renamed VulkanDevice once the placeholder one is gone
	class VkDevice
	{
	public:

		VkDevice(vk::PhysicalDevice&& physDev);

		const auto&	Properties() const
		{ return m_properties; }

		const auto& Features() const
		{
			return m_features;
		}

		uint32_t	GetGraphicsScore() const;




	private:
		bool		HasRequiredGraphicsFeatures() const;
		uint32_t	RateGraphicsProperties() const;
		bool		HasRequiredGraphicsQueueFamily() const;

		// No Unique handle for Physical Device because it doesn't need to be destroyed
		vk::PhysicalDevice	m_physicalDevice;
		vk::UniqueDevice	m_logicalDevice;

		vk::PhysicalDeviceProperties				m_properties;
		vk::PhysicalDeviceFeatures					m_features;
		std::vector<vk::QueueFamilyProperties>		m_queueFamilyProps;
	};


	/**
	 * @brief A class meant to represent the available devices detected by our instance.
	 */
	class VulkanDeviceList
	{
	public:

		VulkanDeviceList() = default;

		bool	Initialize(const vk::Instance& instance);

		void	PopulateDevices(const vk::Instance& instance);

	private:

		VkDevice*	PickGraphicsDevice();

		std::vector<VkDevice>	m_devices;
		VkDevice*				m_graphicsDevice = nullptr;

	};


}

#endif // MOE_VULKAN

