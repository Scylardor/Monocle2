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


	void VulkanSwapchainTexture::Free(MyVkDevice& device)
	{
		// we don't need to destroy the image and there is no sampler for swapchain images.
		if (m_imageView)
			device->destroyImageView(m_imageView);
	}
}


#endif // MOE_VULKAN
