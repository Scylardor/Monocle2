


#pragma once
#ifdef MOE_VULKAN
#include "Graphics/Vulkan/Instance/VulkanInstance.h"
#include "Graphics/Vulkan/Devices/VulkanDevices.h"

namespace moe
{



	class VulkanRHI
	{
	public:
		VulkanRHI() = default;
		~VulkanRHI();

		bool	Initialize(VulkanInstance::CreationParams&& instanceParams);

	private:

		VulkanInstance	m_instance;

		VulkanDevices	m_devices;

	};



}

#endif