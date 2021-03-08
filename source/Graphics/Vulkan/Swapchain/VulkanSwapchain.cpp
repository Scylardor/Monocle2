#ifdef MOE_VULKAN

#include "VulkanSwapchain.h"

#include "Graphics/Vulkan/Surface/VulkanSurfaceProvider.h"

namespace moe
{
	VulkanSwapchain::VulkanSwapchain(const vk::Instance& instance, IVulkanSurfaceProvider& surfaceProvider) :
		m_surfaceProvider(&surfaceProvider)
	{
		VkSurfaceKHR surface = m_surfaceProvider->GetSurface(instance);
		m_surface = vk::UniqueSurfaceKHR(surface, instance);
	}
}


#endif // MOE_VULKAN
