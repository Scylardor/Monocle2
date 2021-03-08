#ifdef MOE_VULKAN

#include "VulkanSwapchain.h"

#include "Graphics/Vulkan/Surface/VulkanSurfaceProvider.h"

namespace moe
{

	bool VulkanSwapchain::Create(const vk::Instance& instance, IVulkanSurfaceProvider& surfaceProvider, vk::SurfaceKHR presentSurface)
	{
		MOE_ASSERT(m_surfaceProvider == nullptr); // should not get called twice on the same swapchain !
		m_surfaceProvider = &surfaceProvider;

		// It would make sense to create the surface here, but because of other shenanigans (device suitability checks),
		// it actually has to be created earlier. Not the cleanest interface, but all we have to do here is keep an unique handle to it.

		m_surface = vk::UniqueSurfaceKHR(presentSurface, instance);

		return true;
	}
}


#endif // MOE_VULKAN
