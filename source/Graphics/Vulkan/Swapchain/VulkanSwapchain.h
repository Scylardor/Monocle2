#pragma once
#ifdef MOE_VULKAN

#include <vulkan/vulkan.hpp>

namespace moe
{
	class IVulkanSurfaceProvider;

	class VulkanSwapchain
	{
	public:

		VulkanSwapchain(const vk::Instance& instance, IVulkanSurfaceProvider& surfaceProvider);


	private:

		vk::UniqueSurfaceKHR	m_surface;
		IVulkanSurfaceProvider* m_surfaceProvider = nullptr;
	};

}


#endif // MOE_VULKAN
