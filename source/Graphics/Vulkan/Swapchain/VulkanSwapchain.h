#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"

#include "VulkanSwapchainFrame.h"

#include "Core/Misc/Literals.h"

namespace moe
{
	class IVulkanSurfaceProvider;

	class VulkanSwapchain
	{
	public:


		VulkanSwapchain() = default;
		~VulkanSwapchain() = default;

		bool		Initialize(vk::Instance instance, const class MyVkDevice& compatibleDevice, IVulkanSurfaceProvider& surfaceProvider, vk::SurfaceKHR presentSurface);

		void		PrepareNewFrame();

		void		SubmitCommandBuffers(vk::CommandBuffer& commandBufferList, uint32_t listSize);

		void		PresentFrame();

		uint32_t	GetImageInFlightIndex() const
		{
			return m_currentImageInFlightIdx;
		}

	private:

		const MyVkDevice& Device() const
		{
			MOE_ASSERT(m_device != nullptr);
			return *m_device;
		}


		vk::Result	AcquireNextImage();

		bool	InitializeFrameList(vk::Format swapChainImageFormat);

		const vk::Semaphore*	GetCurrentFramePresentCompleteSemaphore() const
		{
			return &m_swapChainFrames[m_currentFrameIdx].PresentCompleteSemaphore.get();
		}

		const vk::Semaphore* GetCurrentFrameRenderCompleteSemaphore() const
		{
			return &m_swapChainFrames[m_currentFrameIdx].RenderCompleteSemaphore.get();
		}

		vk::Fence GetCurrentFrameQueueSubmitFence() const
		{
			return m_swapChainFrames[m_currentFrameIdx].QueueSubmitFence.get();
		}

		vk::SwapchainCreateInfoKHR			GenerateSwapchainCreateInfo(const MyVkDevice& compatibleDevice, vk::SurfaceKHR surface);

		static	vk::SurfaceFormatKHR		PickSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) ;
		static	vk::PresentModeKHR			PickPresentMode(const std::vector<vk::PresentModeKHR>& availableModes) ;
		static vk::Extent2D					ComputeSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, IVulkanSurfaceProvider& surfaceProvider) ;
		static uint32_t						PickImageCount(const vk::SurfaceCapabilitiesKHR& capabilities) ;

		vk::UniqueSurfaceKHR	m_surface;
		vk::UniqueSwapchainKHR	m_swapChain;

		std::vector<VulkanSwapchainFrame>	m_swapChainFrames;
		uint32_t							m_currentFrameIdx = 0;

		std::vector<VulkanSwapchainImage>	m_imagesInFlight;
		uint32_t							m_currentImageInFlightIdx = 0;

		bool	m_surfaceWasResized = false;

		// Bound resources
		IVulkanSurfaceProvider* m_surfaceProvider = nullptr;
		const MyVkDevice* m_device = nullptr;

		// TODO : Making it a static const for now - make it configurable later
		static const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

	};

}


#endif // MOE_VULKAN
