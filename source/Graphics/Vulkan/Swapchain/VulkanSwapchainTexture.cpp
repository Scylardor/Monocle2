#ifdef MOE_VULKAN

#include "VulkanSwapchainTexture.h"

namespace moe
{
	VulkanSwapchainTexture::VulkanSwapchainTexture(vk::Image image, vk::UniqueImageView&& view,
		const VulkanTextureBuilder& builder) :
		VulkanTexture(builder)
	{
		Image = image;
		m_imageView = std::move(view);
	}
}


#endif // MOE_VULKAN
