#ifdef MOE_VULKAN

#include "VulkanSwapchainTexture.h"
#include "Graphics/Vulkan/Device/VulkanDevice.h"

namespace moe
{
	VulkanSwapchainTexture::VulkanSwapchainTexture(MyVkDevice& device, vk::Image image, vk::ImageView view,
		const VulkanTextureBuilder& builder) :
		VulkanTexture(device, builder)
	{
		Image = image;
		m_imageView = view;
	}


	VulkanSwapchainTexture::~VulkanSwapchainTexture()
	{
		// we don't need to destroy the image and there is no sampler for swapchain images.
		if (m_imageView)
			(*m_device)->destroyImageView(m_imageView);

		// set to nullptr so the vulkan texture won't try to free itself
		m_device = nullptr;
	}


}


#endif // MOE_VULKAN
