#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

#include "Graphics/Vulkan/Texture/VulkanTexture.h"

namespace moe
{
	class MyVkDevice;

	class VulkanTextureAllocator
	{
	public:

		VulkanTextureAllocator() = default;

		VulkanTextureAllocator(MyVkDevice& device) :
			m_device(&device)
		{}



		[[nodiscard]] VulkanTexture	AllocateImage(VulkanTextureBuilder& builder);

		[[nodiscard]] bool	HasTilingFeatureFor(vk::Format imageFormat, vk::ImageTiling tilingMode, vk::FormatFeatureFlags tilingFeature) const;

		[[nodiscard]] float	GetMaxSupportedAnisotropy() const;

		// Find the maximum sample count that is supported for BOTH color and depth formats
		// If we also wanna use the stencil, check the stencil limits too.
		[[nodiscard]] vk::SampleCountFlagBits	FindMaxUsableColorDepthSampleCount(bool useStencil = false) const;

	protected:

	private:

		[[nodiscard]] const vk::FormatProperties&	FindImageFormatProperties(vk::Format imageFormat) const;

		[[nodiscard]] static vk::FormatFeatureFlags	GetFormatFeaturesForUsage(vk::ImageUsageFlags usage);

		[[nodiscard]] vk::ImageTiling	GetFormatTilingMode(vk::Format imageFormat, vk::FormatFeatureFlags imageFeatures) const;

		// Cache to avoid asking the device each time
		mutable std::unordered_map<vk::Format, vk::FormatProperties>	m_formatProperties{};

		MyVkDevice*	m_device;
	};

}


#endif // MOE_VULKAN
