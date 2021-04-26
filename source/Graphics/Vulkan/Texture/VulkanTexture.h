#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"
#include "Graphics/Vulkan/Allocators/MemoryAllocator.h"

#include "Graphics/Vulkan/Buffer/VulkanBuffer.h"

namespace moe
{
	struct VulkanTextureBuilder;


	class VulkanTexture
	{
	public:
		// if this anisotropy value is set,
		// automatically set maxAnisotropy to the maximum value allowed by the device.
		static const auto MAX_ANISOTROPY = 0;

		// if this maxLod value is used, use mipLevels as maxLod.
		static const auto MAX_MIP_LOD = 0;

		// Set mipLevels to 0 to let the engine
		// generate as many mips as possible.
		// Set the mipLevels value to 1 for no mips.
		static const auto AUTOMATIC_MIP_LEVELS = 0;

		VulkanTexture() = default;

		VulkanTexture(vk::UniqueImage&& img, vk::UniqueImageView&& imgView, VulkanMemoryBlock&& memory, const VulkanTextureBuilder& builder);

		static VulkanTexture	Create2DTexture(MyVkDevice& device, VulkanTextureBuilder& builder);
		static VulkanTexture	Create3DTexture(MyVkDevice& device, VulkanTextureBuilder& builder);
		static VulkanTexture	CreateCubemap(MyVkDevice& device, VulkanTextureBuilder& builder);
		static VulkanTexture	CreateColorAttachment(MyVkDevice& device, VulkanTextureBuilder& builder);
		static VulkanTexture	CreateDepthStencilAttachment(MyVkDevice& device, VulkanTextureBuilder& builder);

		static VulkanTexture	Create2DFromFile(MyVkDevice& device, std::string_view filename, VulkanTextureBuilder& builder);

		static VulkanTexture	Create2DFromData(MyVkDevice& device, const byte_t* imageData, size_t imageSize, VulkanTextureBuilder& builder);

		void	FillStagingBuffer(MyVkDevice& device, const byte_t* imageData, vk::DeviceSize imageSize);

		void	UploadStagingBuffer(vk::CommandBuffer commandBuffer);

		void	TransitionLayout(MyVkDevice& device, vk::ImageLayout newLayout, vk::CommandBuffer* commandBuffer = nullptr);

		void	GenerateMipmaps(MyVkDevice& device, vk::CommandBuffer commandBuffer, vk::ImageTiling tilingMode);

		const vk::DescriptorImageInfo&	DescriptorImageInfo() const
		{
			return m_descriptorInfo;
		}

		static uint32_t					ComputeNumberOfMipmapsForDimensions(uint32_t width, uint32_t height);
		static vk::ImageAspectFlags		FindImageAspect(vk::ImageUsageFlags usage, vk::Format format);
		static bool						FormatHasStencil(vk::Format format);

	protected:

	private:

		void	TransitionLayoutImpl(vk::ImageLayout newLayout, vk::CommandBuffer commandBuffer);

		void	CreateSampler(MyVkDevice& device, VulkanTextureBuilder& builder);

		void	UpdateDescriptorInfo();


		vk::UniqueImage			m_image{};
		VulkanMemoryBlock		m_imageMemory;
		vk::UniqueImageView		m_imageView{};
		vk::UniqueSampler		m_sampler{};

		BufferHandles			m_staging{}; // Needed for (possibly asynchronous) memory transfers. The buffer has to live until the transfer command is sent.
		vk::DescriptorImageInfo	m_descriptorInfo{};

		// These are "scratchpad information" to make it easier to work with textures.
		vk::Extent3D            m_dimensions{0, 0, 1};
		uint32_t                m_mipLevels = 0;
		uint32_t				m_layerCount = 1;
		vk::SampleCountFlagBits	m_numSamples = vk::SampleCountFlagBits::e1;
		vk::Format				m_format = vk::Format::eR8G8B8A8Unorm;
		vk::ImageUsageFlags		m_usage{};
		vk::ImageLayout			m_layout = vk::ImageLayout::eUndefined;
	};

	struct VulkanTextureBuilder
	{
		explicit VulkanTextureBuilder()
		{
			SetFormat(vk::Format::eR8G8B8A8Srgb);
			ImageCreateInfo.arrayLayers = 1;
			ImageCreateInfo.extent.depth = 1;
			ImageCreateInfo.mipLevels = VulkanTexture::AUTOMATIC_MIP_LEVELS;
			ImageCreateInfo.samples = vk::SampleCountFlagBits::e1;
			ImageViewCreateInfo.subresourceRange.layerCount = 1;
			ImageViewCreateInfo.components = { vk::ComponentSwizzle::eR,  vk::ComponentSwizzle::eG,  vk::ComponentSwizzle::eB,  vk::ComponentSwizzle::eA };

			// There is no reason not to use this unless performance is a concern.
			// Instead of enforcing the availability of anisotropic filtering, it's also possible to simply not use it by conditionally setting:
			//		samplerInfo.anisotropyEnable = VK_FALSE;
			//		samplerInfo.maxAnisotropy = 1.0f;
			SamplerCreateInfo.anisotropyEnable = true;
			SamplerCreateInfo.maxAnisotropy = VulkanTexture::MAX_ANISOTROPY;

			SamplerCreateInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;

		}


		VulkanTextureBuilder&	SetDimensions(uint32_t width, uint32_t height, uint32_t depth = 1)
		{
			ImageCreateInfo.extent = vk::Extent3D{ width, height, depth };
			return *this;
		}


		VulkanTextureBuilder&	SetFormat(vk::Format format)
		{
			ImageCreateInfo.format = format;
			ImageViewCreateInfo.format = format;
			return *this;
		}


		VulkanTextureBuilder& SetSamplesCount(vk::SampleCountFlagBits sampleCount)
		{
			ImageCreateInfo.samples = sampleCount;
			return *this;
		}

		VulkanTextureBuilder& SetSamplerModeU(vk::SamplerAddressMode samU = vk::SamplerAddressMode::eRepeat)
		{
			SamplerCreateInfo.addressModeU = samU;
			return *this;
		}

		VulkanTextureBuilder& SetSamplerModeV(vk::SamplerAddressMode samV = vk::SamplerAddressMode::eRepeat)
		{
			SamplerCreateInfo.addressModeV = samV;
			return *this;
		}

		VulkanTextureBuilder& SetSamplerModeUV(vk::SamplerAddressMode samU = vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode samV = vk::SamplerAddressMode::eRepeat)
		{
			SamplerCreateInfo.addressModeU = samU;
			SamplerCreateInfo.addressModeV = samV;
			return *this;
		}


		VulkanTextureBuilder& SetSamplerModeW(vk::SamplerAddressMode samW = vk::SamplerAddressMode::eRepeat)
		{
			SamplerCreateInfo.addressModeW = samW;
			return *this;
		}

		VulkanTextureBuilder& SetSamplerModeUVW(vk::SamplerAddressMode samUVW = vk::SamplerAddressMode::eRepeat)
		{
			SamplerCreateInfo.addressModeU = SamplerCreateInfo.addressModeV = SamplerCreateInfo.addressModeW = samUVW;
			return *this;
		}

		VulkanTextureBuilder& SetSamplerMode(vk::SamplerAddressMode samU = vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode samV = vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode samW = vk::SamplerAddressMode::eRepeat)
		{
			SamplerCreateInfo.addressModeU = samU;
			SamplerCreateInfo.addressModeV = samV;
			SamplerCreateInfo.addressModeW = samW;
			return *this;
		}


		VulkanTextureBuilder& SetSamplerMagFilter(vk::Filter filter)
		{
			SamplerCreateInfo.magFilter = filter;
			return *this;
		}

		VulkanTextureBuilder& SetSamplerMinFilter(vk::Filter filter)
		{
			SamplerCreateInfo.minFilter = filter;
			return *this;
		}


		VulkanTextureBuilder& SetSamplerAnisotropy(bool enabled, float maxAniso = VulkanTexture::MAX_ANISOTROPY)
		{
			SamplerCreateInfo.anisotropyEnable = enabled;
			if (false == enabled)
				SamplerCreateInfo.maxAnisotropy = 1.f;
			else
				SamplerCreateInfo.maxAnisotropy = maxAniso;
			return *this;
		}


		VulkanTextureBuilder& SetMipmapsCount(uint32_t mipLevels)
		{
			ImageCreateInfo.mipLevels = mipLevels;
			ImageViewCreateInfo.subresourceRange.levelCount = mipLevels;
			return *this;
		}


		VulkanTextureBuilder& SetSamplerMipmapMode(vk::SamplerMipmapMode mipmapMode)
		{
			SamplerCreateInfo.mipmapMode = mipmapMode;
			return *this;
		}


		VulkanTextureBuilder& SetSamplerMipmapLod(float lodBias, float minLod, float maxLod = VulkanTexture::MAX_MIP_LOD)
		{
			SamplerCreateInfo.mipLodBias = lodBias;
			SamplerCreateInfo.minLod = minLod;
			SamplerCreateInfo.maxLod = maxLod;

			if (maxLod == VulkanTexture::MAX_MIP_LOD)
			{
				SamplerCreateInfo.maxLod = (float) ImageCreateInfo.mipLevels;
			}

			return *this;
		}


		VulkanTextureBuilder& SetSamplerCompareOp(bool enabled, vk::CompareOp op)
		{
			SamplerCreateInfo.compareEnable = enabled;
			SamplerCreateInfo.compareOp = op;
			return *this;
		}


		VulkanTextureBuilder& SetSamplerBorderColor(vk::BorderColor borderColor)
		{
			SamplerCreateInfo.borderColor = borderColor;
			return *this;
		}

		vk::ImageCreateInfo		ImageCreateInfo{};
		vk::SamplerCreateInfo	SamplerCreateInfo{};
		vk::ImageViewCreateInfo	ImageViewCreateInfo{};
	};
}


#endif // MOE_VULKAN
