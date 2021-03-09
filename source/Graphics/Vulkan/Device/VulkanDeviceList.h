#pragma once

#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

#include "VulkanDevice.h"

namespace moe
{


	/**
	 * @brief A class meant to represent the available devices detected by our instance.
	 */
	class VulkanDeviceList
	{
	public:

		VulkanDeviceList() = default;

		bool	Initialize(const vk::Instance& instance);

		void	PopulateDevices(const vk::Instance& instance);

		MyVkDevice* PickGraphicsDevice(vk::SurfaceKHR presentSurface);

		MyVkDevice*	GetGraphicsDevice() const
		{
			return m_graphicsDevice;
		}

	private:


		std::vector<MyVkDevice>	m_devices;
		MyVkDevice*				m_graphicsDevice = nullptr;


	};


}

#endif // MOE_VULKAN

