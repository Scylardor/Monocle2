#ifdef MOE_VULKAN

#include "VulkanDevices.h"

namespace moe
{
	VkDevice::VkDevice(vk::PhysicalDevice&& physDev) :
		m_physicalDevice(std::move(physDev))
	{
		m_properties = m_physicalDevice.getProperties();
	}

	bool VulkanDevices::Initialize(const vk::Instance& instance)
	{
		PopulateDevices(instance);

		m_graphicsDevice = PickGraphicsDevice();

		return true;
	}


	void VulkanDevices::PopulateDevices(const vk::Instance& instance)
	{
		std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
		MOE_DEBUG_ASSERT(false == physicalDevices.empty());

		m_devices.reserve(physicalDevices.size());
		for (auto& vkDevice : physicalDevices)
		{
			auto& device = m_devices.emplace_back(std::move(vkDevice));
			auto& props = device.Properties();

			MOE_INFO(moe::ChanGraphics, "Found physical device %s (%s | Vulkan v%i.%i.%i | Device #%lu)",
				props.deviceName.data(),
				vk::to_string(props.deviceType),
				VK_VERSION_MAJOR(props.apiVersion), VK_VERSION_MINOR(props.apiVersion), VK_VERSION_PATCH(props.apiVersion),
				props.deviceID
			);
		}
	}


	VkDevice* VulkanDevices::PickGraphicsDevice()
	{
		return nullptr;
	}


	uint32_t VulkanDevices::ComputeDeviceGraphicsRate(VkDevice& ) const
	{
		return 0;
	}
}

#endif // MOE_VULKAN
