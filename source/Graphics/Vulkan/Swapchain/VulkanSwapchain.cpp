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

		vk::SwapchainCreateInfoKHR createInfo = GenerateSwapchainCreateInfo(compatibleDevice, m_surface.get());

		m_swapChain = compatibleDevice->createSwapchainKHRUnique(createInfo);
		MOE_ASSERT(m_swapChain.get());

		bool ok = m_frameList.Initialize(compatibleDevice, m_swapChain.get(), createInfo.imageFormat);
		MOE_ASSERT(ok);

		return ok;
	}


	void VulkanSwapchain::DrawFrame()
	{
		PrepareNewFrame();
	}


	void VulkanSwapchain::PrepareNewFrame()
	{

	}


	void VulkanSwapchain::AcquireNextImage()
	{

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
			createInfo.queueFamilyIndexCount = 2;
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
