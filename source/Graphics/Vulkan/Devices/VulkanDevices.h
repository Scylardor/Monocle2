
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


	private:
		// No Unique handle for Physical Device because it doesn't need to be destroyed
		vk::PhysicalDevice	m_physicalDevice;
		vk::UniqueDevice	m_logicalDevice;

		vk::PhysicalDeviceProperties	m_properties;

	};


	/**
	 * @brief A class meant to represent the available devices detected by our instance.
	 */
	class VulkanDevices
	{
	public:

		VulkanDevices() = default;

		bool	Initialize(const vk::Instance& instance);

		void	PopulateDevices(const vk::Instance& instance);

	private:

		VkDevice*	PickGraphicsDevice();

		uint32_t		ComputeDeviceGraphicsRate(VkDevice& device) const;

		std::vector<VkDevice>	m_devices;
		VkDevice*				m_graphicsDevice = nullptr;

	};


}

#endif // MOE_VULKAN

