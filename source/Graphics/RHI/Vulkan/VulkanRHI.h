
#pragma once
#ifdef MOE_VULKAN
#include "Graphics/Vulkan/Instance/VulkanInstance.h"
#include "Graphics/Vulkan/Device/VulkanDeviceList.h"


namespace moe
{

	class VulkanRHI
	{
	public:
		VulkanRHI() = default;
		~VulkanRHI();

		bool	Initialize(VulkanInstance::CreationParams&& instanceParams);

		MyVkDevice* InitializeGraphicsDevice(vk::SurfaceKHR targetSurface);

		const vk::Instance& GetInstance() const
		{
			return m_instance.Instance();
		}

		const MyVkDevice&	GetGraphicsDevice() const
		{
			return *m_devices.GetGraphicsDevice();
		}

	private:

		VulkanInstance		m_instance;

		VulkanDeviceList	m_devices;

	};



}

#endif