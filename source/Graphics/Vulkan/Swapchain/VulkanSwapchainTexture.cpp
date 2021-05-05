#ifdef MOE_VULKAN

#include "VulkanSwapchainTexture.h"

namespace moe
{
	VulkanSwapchainTexture::VulkanSwapchainTexture(MyVkDevice& device, vk::Image image, vk::UniqueImageView&& view,
		const VulkanTextureBuilder& builder) :
		VulkanTexture(device, builder)
	{
		Image = image;
		m_imageView = std::move(view);
	}
}


#endif // MOE_VULKAN
