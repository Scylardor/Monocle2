
#ifdef MOE_VULKAN

#include "VulkanSwapchainFrame.h"
#include "Graphics/Vulkan/Device/VulkanDevice.h"
namespace moe
{
	VulkanSwapchainImage::VulkanSwapchainImage(MyVkDevice& device, vk::Image swapchainImage, const VulkanTextureBuilder& swapchainTextureBuilder) :
		Texture(device, swapchainImage,
			device.CreateImageView(swapchainImage, swapchainTextureBuilder.ImageCreateInfo.format, vk::ImageAspectFlagBits::eColor, 1),
			swapchainTextureBuilder)
	{
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


	void VulkanSwapchainImage::Free(MyVkDevice& device)
	{
		Texture.Free(device);
	}


	VulkanSwapchainFrame::VulkanSwapchainFrame(const MyVkDevice& device)
	{
		RenderCompleteSemaphore = device->createSemaphore(vk::SemaphoreCreateInfo{});
		PresentCompleteSemaphore = device->createSemaphore(vk::SemaphoreCreateInfo{});

		vk::FenceCreateInfo createInfo{};
		createInfo.flags = vk::FenceCreateFlagBits::eSignaled; // Create the fence in 'signaled' state or waiting for it the first time will hang forever.
		QueueSubmitFence = device->createFence(createInfo);
	}


	bool VulkanSwapchainFrame::WaitForSubmitFence(const MyVkDevice& device)
	{
		static const auto NO_TIMEOUT = UINT64_MAX;
		static const auto waitAll = VK_TRUE;

		vk::Fence* imageSubmitFence = &QueueSubmitFence;
		MOE_VK_CHECK(device->waitForFences(1, imageSubmitFence, waitAll, NO_TIMEOUT));


		return true;
	}


	void VulkanSwapchainFrame::Free(const MyVkDevice& device)
	{
		device->destroySemaphore(PresentCompleteSemaphore);
		device->destroySemaphore(RenderCompleteSemaphore);
		device->destroyFence(QueueSubmitFence);
	}
}


#endif // MOE_VULKAN
