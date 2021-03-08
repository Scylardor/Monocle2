
#pragma once
#ifdef MOE_VULKAN
#include "Graphics/Vulkan/Instance/VulkanInstance.h"
#include "Graphics/Vulkan/Devices/VulkanDeviceList.h"

#include <vulkan/vulkan.h>

namespace moe
{
	class VulkanRHI
	{
	public:
		VulkanRHI() = default;
		~VulkanRHI();

		bool	Initialize(VulkanInstance::CreationParams&& instanceParams);

		VkInstance GetVkInstance() const
		{
			return m_instance.Instance();
		}


	private:

		VulkanInstance		m_instance;

		VulkanDeviceList	m_devices;

	};



}

#endif