
#include "VulkanSwapchainImage.h"
#ifdef MOE_VULKAN

#include "VulkanSwapchainFrame.h"
#include "Graphics/Vulkan/Device/VulkanDevice.h"
namespace moe
{
	bool SwapchainFrame::Initialize(const MyVkDevice& device, vk::Image frameImage, vk::Format swapChainImageFormat)
	{
		FrameImage.Handle = frameImage;

		// Create the image view here for now. TODO: use a Texture Factory for that
		FrameImage.View = device.CreateImageView(FrameImage.Handle, swapChainImageFormat, vk::ImageAspectFlagBits::eColor, 1);

		return true;
	}



	bool SwapchainFrameList::Initialize(const MyVkDevice& device, vk::SwapchainKHR swapChain, vk::Format swapChainImageFormat)
	{
		auto newSwapchainImages = device->getSwapchainImagesKHR(swapChain);

		MOE_ASSERT(false == newSwapchainImages.empty());

		ClearImages(*device);
		m_swapChainImages.resize(newSwapchainImages.size());

		int iFrame = 0;
		bool ok = true; // start optimistic
		for (auto& frame : m_swapChainImages)
		{
			ok &= frame.Initialize(device, newSwapchainImages[iFrame], swapChainImageFormat);
			MOE_ASSERT(ok);

			iFrame++;
		}

		return ok;
	}


	void SwapchainFrameList::ClearImages(vk::Device device)
	{
		for (auto& frame : m_swapChainImages)
		{
			device.destroyImage(frame.FrameImage.Handle);
		}
		m_swapChainImages.clear(); // destroy unique handles
	}
}


#endif // MOE_VULKAN
