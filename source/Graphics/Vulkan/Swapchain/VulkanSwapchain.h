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

		bool		Initialize(vk::Instance instance, class MyVkDevice& compatibleDevice, IVulkanSurfaceProvider& surfaceProvider, vk::SurfaceKHR presentSurface);

		void		PrepareNewFrame();

		void		SubmitCommandBuffers(vk::CommandBuffer* commandBufferList, uint32_t listSize);

		void		PresentFrame();

		uint32_t	GetImageInFlightIndex() const
		{
			return m_currentImageInFlightIdx;
		}

		uint32_t	GetImagesInFlightCount() const
		{
			return (uint32_t) m_imagesInFlight.size();
		}


		uint32_t	GetMaxFramesInFlight() const
		{
			return MAX_FRAMES_IN_FLIGHT;
		}

		uint32_t	GetFrameIndex() const
		{
			return m_currentFrameIdx;
		}

		vk::Format	GetColorAttachmentFormat() const
		{
			return m_imageFormat;
		}

		vk::Format	GetDepthAttachmentFormat() const
		{
			return m_depthStencilAttachment.Format();
		}


		vk::ImageView	GetColorAttachmentView(int imageIndex) const
		{
			MOE_ASSERT(imageIndex < m_imagesInFlight.size());
			return m_imagesInFlight[imageIndex].View.get();
		}

		vk::ImageView	GetDepthAttachmentView() const
		{
			return m_depthStencilAttachment.DescriptorImageInfo().imageView;
		}

		VkExtent2D	GetSwapchainImageExtent() const
		{
			return m_imagesExtent;
		}


	private:

		const MyVkDevice& Device() const
		{
			MOE_ASSERT(m_device != nullptr);
			return *m_device;
		}


		vk::Result	AcquireNextImage();

		bool	InitializeFrameList(vk::Format swapChainImageFormat);

		void	InitializeDepthStencilAttachment();

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

		VulkanTexture						m_depthStencilAttachment{};

		VkExtent2D							m_imagesExtent{};

		bool	m_surfaceWasResized = false;

		vk::Format							m_imageFormat;

		// Bound resources
		IVulkanSurfaceProvider* m_surfaceProvider = nullptr;
		MyVkDevice* m_device = nullptr;

		// TODO : Making it a static const for now - make it configurable later
		static const uint32_t MAX_FRAMES_IN_FLIGHT;

	};

}


#endif // MOE_VULKAN
