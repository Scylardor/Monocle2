#ifdef MOE_VULKAN

#include "VulkanSwapchain.h"

#include "Graphics/Vulkan/Surface/VulkanSurfaceProvider.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"

namespace moe
{


	bool VulkanSwapchain::Initialize(vk::Instance instance, const MyVkDevice& compatibleDevice, IVulkanSurfaceProvider& surfaceProvider, vk::SurfaceKHR presentSurface)
	{
		MOE_ASSERT(m_surfaceProvider == nullptr); // should not get called twice on the same swapchain !

		// Bind external resources useful to this swapchain
		m_surfaceProvider = &surfaceProvider;
		m_device = &compatibleDevice;

		// It would make sense to create the surface here, but because of other shenanigans (device suitability checks),
		// it actually has to be created earlier. Not the cleanest interface, but all we have to do here is keep an unique handle to it.
		m_surface = vk::UniqueSurfaceKHR(presentSurface, instance);
		MOE_ASSERT(m_surface.get());

		m_surfaceProvider->RegisterSurfaceResizeCallback([this](Width_t, Height_t)
		{
			m_surfaceWasResized = true;
		});

		vk::SwapchainCreateInfoKHR createInfo = GenerateSwapchainCreateInfo(compatibleDevice, m_surface.get());

		m_swapChain = compatibleDevice->createSwapchainKHRUnique(createInfo);
		MOE_ASSERT(m_swapChain.get());

		bool ok = InitializeFrameList(createInfo.imageFormat);
		MOE_ASSERT(ok);

		// Image format was validated (swap chain successfully created) : store it somewhere (useful to create framebuffers using swap chain images)
		m_imageFormat = createInfo.imageFormat;
		m_imagesExtent = createInfo.imageExtent;

		PrepareNewFrame();
		return ok;
	}


	void VulkanSwapchain::SubmitCommandBuffers(vk::CommandBuffer* commandBufferList, uint32_t listSize)
	{
		MOE_ASSERT(commandBufferList != nullptr && listSize > 0);

		vk::SubmitInfo submitInfo{};

		submitInfo.pCommandBuffers = commandBufferList;
		submitInfo.commandBufferCount = listSize;

		// Wait for the "present is done" semaphore of this frame to be signalled, so that it is safe to reuse for drawing.
		submitInfo.pWaitSemaphores = GetCurrentFramePresentCompleteSemaphore();
		submitInfo.waitSemaphoreCount = 1;

		// Don't let the stage writing to the image start before the present semaphore was signalled.
		// The submit is going to wait before starting the color attachment output ("write to the image") stage.
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		submitInfo.setPWaitDstStageMask(waitStages);

		// Once the submit is finished, signal it to the render finished semaphore of this frame.
		submitInfo.pSignalSemaphores = GetCurrentFrameRenderCompleteSemaphore();
		submitInfo.signalSemaphoreCount = 1;

		// The current image fence might still be in signaled state. Reset it so the submit doesn't wait on it forever.
		// It's best to do it right before actually using the fence:
		vk::Fence inFlightImageFence = GetCurrentFrameQueueSubmitFence();
		MOE_VK_CHECK(Device()->resetFences(1, &inFlightImageFence));

		// TODO : should really move the queues in the swapchain.
		MOE_VK_CHECK(Device().GraphicsQueue().submit(1, &submitInfo, inFlightImageFence));
	}


	void VulkanSwapchain::PresentFrame()
	{
		vk::PresentInfoKHR presentInfo{};

		// Wait for rendering to be done before trying to present.
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = GetCurrentFrameRenderCompleteSemaphore();

		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_swapChain.get();
		presentInfo.pImageIndices = &m_currentImageInFlightIdx;

		vk::Result result = Device().PresentQueue().presentKHR(&presentInfo);

		// vkQueuePresentKHR returns the same values than vkAcquireNextImageKHR with the same meaning.
		// We also recreate the swap chain if it is suboptimal, because we want the best possible result.
		// Regarding framebuffer resized : It is important to check this after vkQueuePresentKHR,
		// to ensure the semaphores are in a consistent state, otherwise a signalled semaphore may never be properly waited upon.
		if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
		{
			MOE_WARNING(moe::ChanGraphics, "Swap chain has become obsolete - recreating it...");
			// TODO: recreate swap chain here.
		}

		m_currentFrameIdx = (m_currentFrameIdx + 1) % MAX_FRAMES_IN_FLIGHT;
	}



	void VulkanSwapchain::PrepareNewFrame()
	{
		// First wait on the fence of the oldest frame that was submitted, basically
		m_swapChainFrames[m_currentFrameIdx].WaitForSubmitFence(*m_device);

		// Now get the index of the next swap chain image we can use
		vk::Result acquireResult = AcquireNextImage();

		switch (acquireResult)
		{
		case vk::Result::eErrorOutOfDateKHR:
			// we cannot use this swap chain anymore.
			MOE_WARNING(moe::ChanGraphics, "Swap chain out of date - recreation in progress");
			// TODO: recreate the swap chain here.
			return;
		case vk::Result::eSuboptimalKHR:
			// don't manage subobtimal here because we successfully acquired the image : do it after present
			MOE_WARNING(moe::ChanGraphics, "Going to present to suboptimal swap chain, please fix ASAP");
			break;
		default:
			MOE_ASSERT(acquireResult == vk::Result::eSuccess); // not supposed to be anything else than success
			break;
		}

		// We don't know which image it is. Perhaps it is still in use by a previous frame.
		// it shouldn't but check just in case. Then give it current frame's fence
		m_imagesInFlight[m_currentImageInFlightIdx].AcquireFence((vk::Device)Device(), GetCurrentFrameQueueSubmitFence());
	}


	vk::Result VulkanSwapchain::AcquireNextImage()
	{
		static const auto NO_TIMEOUT = UINT64_MAX;
		// Use no fence because we use a semaphore instead (better for performance)
		vk::Result acqRes = (Device()->acquireNextImageKHR(m_swapChain.get(), NO_TIMEOUT, *GetCurrentFramePresentCompleteSemaphore(), vk::Fence(), &m_currentImageInFlightIdx));
		MOE_ASSERT(m_currentImageInFlightIdx < m_imagesInFlight.size());

		return acqRes;
	}


	bool VulkanSwapchain::InitializeFrameList(vk::Format swapChainImageFormat)
	{
		m_swapChainFrames.clear(); // destroys unique handles
		m_swapChainFrames.reserve(MAX_FRAMES_IN_FLIGHT);
		for (int iFrame = 0; iFrame < MAX_FRAMES_IN_FLIGHT; iFrame++)
		{
			m_swapChainFrames.emplace_back(Device());
		}

		auto newSwapchainImages = Device()->getSwapchainImagesKHR(m_swapChain.get());
		MOE_ASSERT(false == newSwapchainImages.empty());

		m_imagesInFlight.clear(); // destroy unique handles, if any. vkImages will be cleaned up by UniqueSwapchain dtor
		m_imagesInFlight.reserve(newSwapchainImages.size());
		for (auto scImage : newSwapchainImages)
		{
			m_imagesInFlight.emplace_back(Device(), scImage, swapChainImageFormat);
		}

		return true;
	}


	vk::SwapchainCreateInfoKHR VulkanSwapchain::GenerateSwapchainCreateInfo(const MyVkDevice& compatibleDevice, vk::SurfaceKHR surface)
	{
		auto& swapchainParams = compatibleDevice.GetSwapchainSupportParameters();
		vk::SurfaceFormatKHR mySurfaceFormat = PickSurfaceFormat(swapchainParams.AvailableSurfaceFormats);

		vk::PresentModeKHR myPresentMode = PickPresentMode(swapchainParams.AvailablePresentModes);

		vk::Extent2D mySwapExtent = ComputeSwapExtent(swapchainParams.SurfaceCapabilities, *m_surfaceProvider);

		uint32_t myImageCount = PickImageCount(swapchainParams.SurfaceCapabilities);

		vk::SwapchainCreateInfoKHR createInfo{};

		createInfo.surface = surface;
		createInfo.minImageCount = myImageCount;
		createInfo.imageFormat = mySurfaceFormat.format;
		createInfo.imageColorSpace = mySurfaceFormat.colorSpace;
		createInfo.imageExtent = mySwapExtent;
		createInfo.imageArrayLayers = 1; // This is always 1 unless you are developing a stereoscopic 3D application.
		createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

		// Specify how to handle swap chain images that will be used across multiple queue families.
		// Concurrent mode = if the graphics queue family is different from the presentation queue.
		// But on most hardware the graphics queue family and presentation queue family are the same.
		// So we should stick to exclusive mode, because concurrent mode requires you to specify at least two distinct queue families.
		MOE_ASSERT(compatibleDevice.HasRequiredGraphicsQueueFamilies());
		auto queueFamilyIndicesArray = compatibleDevice.GetQueueFamilyIndicesArray();
		if (compatibleDevice.HasUnifiedGraphicsAndPresentQueues())
		{
			createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		}
		else
		{
			createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			createInfo.queueFamilyIndexCount = (uint32_t) queueFamilyIndicesArray.size();
			createInfo.pQueueFamilyIndices = queueFamilyIndicesArray.data();
		}

		// Don't apply any pre-transform to the swap chain
		createInfo.preTransform = swapchainParams.SurfaceCapabilities.currentTransform;

		// The compositeAlpha field specifies if the alpha channel should be used
		// for blending with other windows in the window system.
		// You'll almost always want to simply ignore the alpha channel, hence VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR.
		createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

		createInfo.presentMode = myPresentMode;

		// clipped = VK_TRUE means that we don't care about the color of pixels that are obscured,
		// for example because another window is in front of them.
		createInfo.clipped = VK_TRUE;

		// TODO : change that during swap chain recreation (when the window is resized for example)
		createInfo.oldSwapchain = vk::SwapchainKHR(); // null old swap chain handle for now

		return createInfo;
	}


	vk::SurfaceFormatKHR VulkanSwapchain::PickSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
	{
		MOE_ASSERT(false == availableFormats.empty()); // not supposed to happen

		// For the color space we'll use SRGB if it is available, because it results in more accurate perceived colors.
		// It is also pretty much the standard color space for images, like the textures we'll use later on.
		// Because of that we should also use an SRGB color format, of which one of the most common ones is VK_FORMAT_B8G8R8A8_SRGB.
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear)
			{
				return availableFormat;
			}
		}

		// just settle for the first available one
		return availableFormats[0];
	}


	vk::PresentModeKHR VulkanSwapchain::PickPresentMode(const std::vector<vk::PresentModeKHR>& availableModes)
	{
		MOE_ASSERT(false == availableModes.empty()); // not supposed to happen

		// VK_PRESENT_MODE_MAILBOX_KHR is typically used to implement triple buffering.
		// Triple buffering is a very nice trade-off because it allows us to avoid tearing
		// while still maintaining a fairly low latency by rendering new images that are
		// as up-to-date as possible right until the vertical blank.
		// So, let's look through the list to see if it's available
		for (const auto& availablePresentMode : availableModes)
		{
			if (availablePresentMode == vk::PresentModeKHR::eMailbox)
			{
				return availablePresentMode;
			}
		}

		// Only the VK_PRESENT_MODE_FIFO_KHR mode is guaranteed to be available,
		// So choose it as the fallback (it's double buffering).
		return vk::PresentModeKHR::eFifo;

	}


	vk::Extent2D VulkanSwapchain::ComputeSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, IVulkanSurfaceProvider& surfaceProvider)
	{
		// Vulkan tells us to match the resolution of the window by setting the width and height in the currentExtent member.
		// However, some window managers do allow us to differ here and this is indicated by setting the width and height
		// in currentExtent to a special value: the maximum value of uint32_t.
		// In that case we'll pick the resolution that best matches the window within the minImageExtent and maxImageExtent bounds.
		if (capabilities.currentExtent.width != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}

		IVulkanSurfaceProvider::SurfaceDimensions extentDimensions = surfaceProvider.GetSurfaceDimensions();

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(extentDimensions.Width),
			static_cast<uint32_t>(extentDimensions.Height)
		};

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}


	uint32_t VulkanSwapchain::PickImageCount(const vk::SurfaceCapabilitiesKHR& capabilities)
	{
		// how many images we would like to have in the swap chain ?
		// Usually sticking to the minimum number does not grant us optimal performance
		// (e.g. if we draw faster than the GPU can render).
		// Therefore it is recommended to request at least one more image than the minimum
		uint32_t imageCount = capabilities.minImageCount + 1;

		// We should also make sure to not exceed the maximum number of images while doing this.
		// Note that 0 is a special value that means that there is no maximum.
		if (capabilities.maxImageCount > 0)
		{
			imageCount = std::min(imageCount ,capabilities.maxImageCount);
		}

		return imageCount;
	}
}


#endif // MOE_VULKAN
