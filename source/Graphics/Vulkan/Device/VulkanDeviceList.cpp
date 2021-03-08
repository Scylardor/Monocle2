#ifdef MOE_VULKAN

#include "VulkanDeviceList.h"

namespace moe
{


	bool VulkanDeviceList::Initialize(const vk::Instance& instance)
	{
		PopulateDevices(instance);

		return true;
	}


	void VulkanDeviceList::PopulateDevices(const vk::Instance& instance)
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


	MyVkDevice* VulkanDeviceList::PickGraphicsDevice(vk::SurfaceKHR presentSurface)
	{
		MyVkDevice* bestCandidate = nullptr;
		uint32_t bestScore = 0;

		for (auto& device : m_devices)
		{
			const uint32_t score = device.GetScoreForPresentSurface(presentSurface);
			if (score > bestScore)
			{
				bestScore = score;
				bestCandidate = &device;
			}
		}

		if (MOE_ASSERT(bestCandidate != nullptr))
		{
			m_graphicsDevice = bestCandidate;

			MOE_INFO(moe::ChanGraphics, "Picked %s as my Graphics Device.",
				m_graphicsDevice->Properties().deviceName.data()
			);
		}

		return m_graphicsDevice;
	}

}

#endif // MOE_VULKAN
