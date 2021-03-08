#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"


namespace moe
{
	class IVulkanSurfaceProvider;

	class VulkanSwapchain
	{
	public:
		VulkanSwapchain() = default;
		~VulkanSwapchain()
		{

		}

		bool	Create(const vk::Instance& instance, IVulkanSurfaceProvider& surfaceProvider, vk::SurfaceKHR presentSurface);


	private:

		vk::UniqueSurfaceKHR	m_surface;
		IVulkanSurfaceProvider* m_surfaceProvider = nullptr;
	};

}


#endif // MOE_VULKAN
