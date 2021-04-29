#include "Core/Misc/moeFile.h"
#ifdef MOE_VULKAN

#include "TextureAllocator.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"

namespace moe
{
	VulkanTexture VulkanTextureAllocator::AllocateImage(VulkanTextureBuilder& builder)
	{
		// Try to figure out reasonable values given the provided parameters.
		if (builder.ImageCreateInfo.mipLevels != 1)
		{
			// we're going to use blits to generate mipmaps, which is considered as a transfer from the image to itself
			// so it's also going to be a transfer source
			builder.ImageCreateInfo.usage |= vk::ImageUsageFlagBits::eTransferSrc;
		}


		const vk::FormatFeatureFlags features = GetFormatFeaturesForUsage(builder.ImageCreateInfo.usage);
		builder.ImageCreateInfo.tiling = GetFormatTilingMode(builder.ImageCreateInfo.format, features);

		// AUTOMATIC_MIP_LEVELS is a magic number to tell the system "generate as many mipmaps as you can"
		if (builder.ImageCreateInfo.mipLevels == VulkanTexture::AUTOMATIC_MIP_LEVELS)
		{
			builder.ImageCreateInfo.mipLevels = VulkanTexture::ComputeNumberOfMipmapsForDimensions(builder.ImageCreateInfo.extent.width, builder.ImageCreateInfo.extent.height);
			builder.ImageViewCreateInfo.subresourceRange.levelCount = builder.ImageCreateInfo.mipLevels;
		}

		vk::UniqueImage image = m_device->createImageUnique(builder.ImageCreateInfo);
		MOE_ASSERT(image);

		VulkanMemoryBlock imageMemory = m_device.MemoryAllocator().AllocateTextureDeviceMemory(image.get());

		builder.ImageViewCreateInfo.format = builder.ImageCreateInfo.format;

		builder.ImageViewCreateInfo.subresourceRange.aspectMask = VulkanTexture::FindImageAspect(builder.ImageCreateInfo.usage, builder.ImageCreateInfo.format);

		builder.ImageViewCreateInfo.image = image.get();

		vk::UniqueImageView imgView = m_device->createImageViewUnique(builder.ImageViewCreateInfo);

		return VulkanTexture(std::move(image), std::move(imgView), std::move(imageMemory), builder);
	}


	bool VulkanTextureAllocator::HasTilingFeatureFor(vk::Format imageFormat, vk::ImageTiling tilingMode, vk::FormatFeatureFlags tilingFeature) const
	{
		const vk::FormatProperties& props = FindImageFormatProperties(imageFormat);

		switch (tilingMode)
		{
		case vk::ImageTiling::eOptimal:
			return (props.optimalTilingFeatures & tilingFeature) == tilingFeature;
		case vk::ImageTiling::eLinear:
			return (props.linearTilingFeatures & tilingFeature) == tilingFeature;
		}

		MOE_ASSERT(false); // not implemented !
		return false;
	}


	float VulkanTextureAllocator::GetMaxSupportedAnisotropy() const
	{
		return m_device.Properties().limits.maxSamplerAnisotropy;
	}


	vk::SampleCountFlagBits VulkanTextureAllocator::FindMaxUsableColorDepthSampleCount(bool useStencil) const
	{
		vk::SampleCountFlags limits = m_device.Properties().limits.framebufferColorSampleCounts;

		if (useStencil)
			limits &= m_device.Properties().limits.framebufferStencilSampleCounts;
		else
			limits &= m_device.Properties().limits.framebufferDepthSampleCounts;

		int& limitsi = reinterpret_cast<int&>(limits); // evil bit twiddling hack!!
		limits &= vk::SampleCountFlagBits((~limitsi) >> 1); // filter and keep only the highest one

		return vk::SampleCountFlagBits(limitsi);
	}


	const vk::FormatProperties& VulkanTextureAllocator::FindImageFormatProperties(vk::Format imageFormat) const
	{
		// emplace does not replace the value if the key already exists.
		auto [formatProps, inserted] = m_formatProperties.emplace(imageFormat, vk::FormatProperties{});
		vk::FormatProperties* propsPtr = &formatProps->second;

		if (inserted) // not found : ask the API
		{
			m_device.PhysicalDevice().getFormatProperties(imageFormat, propsPtr);
		}

		return *propsPtr;
	}


	vk::FormatFeatureFlags VulkanTextureAllocator::GetFormatFeaturesForUsage(vk::ImageUsageFlags usage)
	{
		vk::FormatFeatureFlags features{};

		if (usage & vk::ImageUsageFlagBits::eSampled)
		{
			features |= vk::FormatFeatureFlagBits::eSampledImage;
		}

		if (usage & vk::ImageUsageFlagBits::eColorAttachment)
		{
			features |= vk::FormatFeatureFlagBits::eColorAttachment;
		}

		if (usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
		{
			features |= vk::FormatFeatureFlagBits::eDepthStencilAttachment;
		}

		return features;
	}


	vk::ImageTiling VulkanTextureAllocator::GetFormatTilingMode(vk::Format imageFormat, vk::FormatFeatureFlags imageFeatures) const
	{
		const vk::FormatProperties& props = FindImageFormatProperties(imageFormat);

		if (props.optimalTilingFeatures & imageFeatures)
		{
			return vk::ImageTiling::eOptimal;
		}
		else if (props.linearTilingFeatures & imageFeatures)
		{
			return vk::ImageTiling::eLinear;
		}

		MOE_ASSERT(false); // No tiling mode supported for those features, not supposed to happen !
		return vk::ImageTiling{};
	}


}


#endif // MOE_VULKAN
