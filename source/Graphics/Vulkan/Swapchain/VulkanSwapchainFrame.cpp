
#ifdef MOE_VULKAN

#include "VulkanSwapchainFrame.h"
#include "Graphics/Vulkan/Device/VulkanDevice.h"
namespace moe
{
	VulkanSwapchainImage::VulkanSwapchainImage(const MyVkDevice& device, vk::Image swapchainImage, vk::Format imageFormat)
	{
		Image = swapchainImage;

		// Create the image view here for now. TODO: use a Texture Factory for that
		View = device.CreateImageView(Image, imageFormat, vk::ImageAspectFlagBits::eColor, 1);

	}


	void VulkanSwapchainImage::AcquireFence(vk::Device device, vk::Fence newInFlightFence)
	{
		// Check if a previous frame is using this image (i.e. there is its fence to wait on)
		if (InFlightFence)
		{
			static const auto NO_TIMEOUT = UINT64_MAX;
			static const auto waitAll = VK_TRUE;
			MOE_VK_CHECK(device.waitForFences(1, &InFlightFence, waitAll, NO_TIMEOUT));
		}

		// This image is now being controlled by the new fence (bound to current frame)
		InFlightFence = newInFlightFence;
	}


	VulkanSwapchainFrame::VulkanSwapchainFrame(const MyVkDevice& device)
	{
		RenderCompleteSemaphore = device->createSemaphoreUnique(vk::SemaphoreCreateInfo{});
		PresentCompleteSemaphore = device->createSemaphoreUnique(vk::SemaphoreCreateInfo{});

		vk::FenceCreateInfo createInfo{};
		createInfo.flags = vk::FenceCreateFlagBits::eSignaled; // Create the fence in 'signaled' state or waiting for it the first time will hang forever.
		QueueSubmitFence = device->createFenceUnique(createInfo);
	}


	bool VulkanSwapchainFrame::WaitForSubmitFence(const MyVkDevice& device)
	{
		static const auto NO_TIMEOUT = UINT64_MAX;
		static const auto waitAll = VK_TRUE;

		vk::Fence* imageSubmitFence = &QueueSubmitFence.get();
		MOE_VK_CHECK(device->waitForFences(1, imageSubmitFence, waitAll, NO_TIMEOUT));

		return true;
	}
}


#endif // MOE_VULKAN
