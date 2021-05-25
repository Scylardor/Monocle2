#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

#include "Graphics/Vulkan/Texture/VulkanTexture.h"


namespace moe
{
	// I'm having a problem with the render pass right now so I'm making this class real quick
	// All I need is a Vulkan Texture that can give me the extent informations !
	// TODO: find a better way to do this.
	class VulkanSwapchainTexture : public VulkanTexture
	{
	public:

		VulkanSwapchainTexture() = default;

		VulkanSwapchainTexture(MyVkDevice& device, vk::Image image, vk::ImageView view, const VulkanTextureBuilder& builder);


		VulkanSwapchainTexture(VulkanSwapchainTexture&& other)
		{
			*this = std::move(other);
		}

		VulkanSwapchainTexture& operator=(VulkanSwapchainTexture&& other)
		{
			if (this != &other)
			{
				Image = other.Image;
				VulkanTexture::operator=(std::move(other));
			}

			return *this;
		}

		~VulkanSwapchainTexture();


		// not a UniqueImage? This isn't a mistake!
		// because those images are retrieved via getSwapchainImagesKHR, they get automatically freed by the UniqueSwapchain handle dtor!
		vk::Image	Image;

	};

}


#endif // MOE_VULKAN
