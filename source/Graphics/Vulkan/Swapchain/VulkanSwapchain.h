#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

#include "VulkanSwapchainFrame.h"


namespace moe
{
	class IVulkanSurfaceProvider;

	class VulkanSwapchain
	{
	public:


		VulkanSwapchain() = default;
		~VulkanSwapchain() = default;

		bool	Initialize(vk::Instance instance, const class MyVkDevice& compatibleDevice, IVulkanSurfaceProvider& surfaceProvider, vk::SurfaceKHR presentSurface);

		void	DrawFrame();

	private:

		void	PrepareNewFrame();

		void	AcquireNextImage();


		vk::SwapchainCreateInfoKHR			GenerateSwapchainCreateInfo(const MyVkDevice& compatibleDevice, vk::SurfaceKHR surface);

		static	vk::SurfaceFormatKHR		PickSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) ;
		static	vk::PresentModeKHR			PickPresentMode(const std::vector<vk::PresentModeKHR>& availableModes) ;
		static vk::Extent2D					ComputeSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, IVulkanSurfaceProvider& surfaceProvider) ;
		static uint32_t						PickImageCount(const vk::SurfaceCapabilitiesKHR& capabilities) ;

		SwapchainFrameList		m_frameList;

		vk::UniqueSurfaceKHR	m_surface;
		vk::UniqueSwapchainKHR	m_swapChain;


		// Bound resources
		IVulkanSurfaceProvider* m_surfaceProvider = nullptr;
		const MyVkDevice* m_device = nullptr;

	};

}


#endif // MOE_VULKAN
