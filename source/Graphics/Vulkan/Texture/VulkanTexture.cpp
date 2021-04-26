
#ifdef MOE_VULKAN

#include "VulkanTexture.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"

#include "Core/Misc/moeFile.h"

#include <STB/stb_image.h>


namespace moe
{
	VulkanTexture::VulkanTexture(vk::UniqueImage&& img, vk::UniqueImageView&& imgView, VulkanMemoryBlock&& memory,
		const VulkanTextureBuilder& builder) :
		m_image(std::move(img)), m_imageView(std::move(imgView)), m_imageMemory(std::move(memory)),
		m_dimensions(builder.ImageCreateInfo.extent),
		m_mipLevels(builder.ImageCreateInfo.mipLevels),
		m_layerCount(builder.ImageCreateInfo.arrayLayers),
		m_numSamples(builder.ImageCreateInfo.samples),
		m_format(builder.ImageCreateInfo.format),
		m_usage(builder.ImageCreateInfo.usage)
	{

	}

	VulkanTexture VulkanTexture::Create2DTexture(MyVkDevice& device, VulkanTextureBuilder& builder)
	{
		// Sampled image = probably a texture we're going to upload data into
		builder.ImageCreateInfo.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
		builder.ImageCreateInfo.imageType = vk::ImageType::e2D;
		builder.ImageViewCreateInfo.viewType = vk::ImageViewType::e2D;

		return device.TextureAllocator().AllocateImage(builder);
	}


	VulkanTexture VulkanTexture::CreateColorAttachment(MyVkDevice& device, VulkanTextureBuilder& builder)
	{
		builder.ImageCreateInfo.usage = vk::ImageUsageFlagBits::eColorAttachment;
		if (builder.ImageCreateInfo.samples != vk::SampleCountFlagBits::e1)
		{
			// multisample color target that will need to be resolved: use transient usage
			builder.ImageCreateInfo.usage |= vk::ImageUsageFlagBits::eTransientAttachment;
		}

		builder.ImageCreateInfo.imageType = vk::ImageType::e2D;
		builder.ImageViewCreateInfo.viewType = vk::ImageViewType::e2D;

		return device.TextureAllocator().AllocateImage(builder);
	}


	VulkanTexture VulkanTexture::CreateDepthStencilAttachment(MyVkDevice& device, VulkanTextureBuilder& builder)
	{
		builder.ImageCreateInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
		builder.ImageCreateInfo.imageType = vk::ImageType::e2D;
		builder.ImageViewCreateInfo.viewType = vk::ImageViewType::e2D;

		return device.TextureAllocator().AllocateImage(builder);
	}


	VulkanTexture VulkanTexture::Create2DFromFile(MyVkDevice& device, std::string_view filename, VulkanTextureBuilder& builder)
	{
		// TODO: Handle only 32bpp RGBA for now but make an equivalent logic than OpenGLGraphicsDevice::CreateTexture2D...
		int width, height, nrChannels;
		void* const imageData = (void*)stbi_load(filename.data(), &width, &height, &nrChannels, STBI_rgb_alpha);
		vk::DeviceSize imageSize = width * height * 4;

		builder.SetDimensions(width, height);

		VulkanTexture newTex = Create2DFromData(device, (const byte_t*)imageData, imageSize, builder);

		// Don't forget to clean up the original pixel array now
		stbi_image_free((void*)imageData);

		return newTex;
	}


	VulkanTexture VulkanTexture::Create2DFromData(MyVkDevice& device, const byte_t* imageData, size_t imageSize, VulkanTextureBuilder& builder)
	{
		VulkanTexture tex2D = Create2DTexture(device, builder);

		tex2D.FillStagingBuffer(device, imageData, imageSize);

		device.ImmediateCommandSubmit([&](vk::CommandBuffer commandBuffer)
			{
				tex2D.TransitionLayout(device, vk::ImageLayout::eTransferDstOptimal, &commandBuffer);

				tex2D.UploadStagingBuffer(commandBuffer);

				tex2D.TransitionLayout(device, vk::ImageLayout::eShaderReadOnlyOptimal, &commandBuffer);

				tex2D.GenerateMipmaps(device, commandBuffer, builder.ImageCreateInfo.tiling);
			});

		tex2D.CreateSampler(device, builder);

		// Now that everything has been created, fill the descriptor info
		tex2D.UpdateDescriptorInfo();

		return tex2D;
	}


	void VulkanTexture::FillStagingBuffer(MyVkDevice& device, const byte_t* imageData, vk::DeviceSize imageSize)
	{
		if (!m_staging.Buffer)
		{
			// Create our staging buffer to upload the image on GPU.
			m_staging = device.BufferAllocator().CreateStagingBuffer(imageSize);
		}

		// We can then directly copy the pixel values that we got from the image loading library to the buffer:
		void* map;
		MOE_VK_CHECK(device->mapMemory(m_staging.MemoryBlock.Memory.get(), m_staging.MemoryBlock.Offset, imageSize, vk::MemoryMapFlags(), &map));
		memcpy(map, imageData, imageSize);
		device->unmapMemory(m_staging.MemoryBlock.Memory.get());
	}


	void VulkanTexture::UploadStagingBuffer(vk::CommandBuffer commandBuffer)
	{
		MOE_ASSERT(m_staging.Buffer);

		vk::BufferImageCopy copyRegion{
			0, 0, 0,
			{ FindImageAspect(m_usage, m_format), 0, 0, m_layerCount },
			{0, 0, 0}, m_dimensions
		};

		commandBuffer.copyBufferToImage(m_staging.Buffer.get(), m_image.get(), m_layout, { copyRegion });
	}


	void VulkanTexture::TransitionLayoutImpl(vk::ImageLayout newLayout,
	                                         vk::CommandBuffer commandBuffer)
	{
		vk::ImageMemoryBarrier barrier{};
		barrier.oldLayout = m_layout;
		barrier.newLayout = newLayout;
		barrier.image = m_image.get();

		// If you are using the barrier to transfer queue family ownership, then these two fields should be the indices of the queue families.
		// They must be set to VK_QUEUE_FAMILY_IGNORED if you don't want to do this (not the default value!).
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = FindImageAspect(m_usage, m_format);

		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = m_mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = m_layerCount;

		vk::PipelineStageFlags sourceStage{};
		vk::PipelineStageFlags destinationStage{};

		barrier.srcAccessMask = vk::AccessFlagBits();
		barrier.dstAccessMask = vk::AccessFlagBits();

		switch (m_layout)
		{
		case vk::ImageLayout::eUndefined:

			sourceStage = vk::PipelineStageFlagBits::eTopOfPipe; // the earliest possible pipeline stage for the pre-barrier operations.
			if (newLayout == vk::ImageLayout::eTransferDstOptimal)
			{
				// Undefined -> transfer destination : transfer writes that don't need to wait on anything
				barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
				destinationStage = vk::PipelineStageFlagBits::eTransfer;
			}
			else if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
			{
				// A depth attachment is both read and written by depth test.
				barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
				// The reading happens in the VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT stage, and the writing in the VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT.
				// We should pick the earliest pipeline stage that matches the specified operations, so that it is ready for usage as depth attachment when it needs to be.
				destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
			}
			break;
		case vk::ImageLayout::eTransferDstOptimal:
			if (newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
			{
				// Transfer destination -> shader reading : shader reads should wait on transfer writes,
				// specifically the fragment shader, because that's where we're going to use textures.
				barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
				barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

				sourceStage = vk::PipelineStageFlagBits::eTransfer;
				destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
			}
			break;
		default:
			MOE_ASSERT(false); // unmanaged transition scenario?
			break;
		}

		if (!sourceStage && !destinationStage && !barrier.srcAccessMask && !barrier.dstAccessMask)
		{
			MOE_WARNING(ChanGraphics, "Vulkan Image Layout transition does not set any barrier access mask / pipeline stage. Potential bug here.");
		}

		// TODO: should be batched in a PipelineSynchronizationBatch or something...
		commandBuffer.pipelineBarrier(
			sourceStage,
			destinationStage,
			vk::DependencyFlagBits(),
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

	}


	void VulkanTexture::CreateSampler(MyVkDevice& device, VulkanTextureBuilder& builder)
	{
		const float maxAniso = device.TextureAllocator().GetMaxSupportedAnisotropy();
		if (builder.SamplerCreateInfo.maxAnisotropy == VulkanTexture::MAX_ANISOTROPY)
		{
			// replace the magic number by the maximum supported anisotropy value
			builder.SamplerCreateInfo.maxAnisotropy = maxAniso;
		}
		else if (builder.SamplerCreateInfo.maxAnisotropy > maxAniso)
		{
			MOE_WARNING(moe::ChanGraphics, "Provided anisotropy value %.2f overflows max supported anisotropy (%.2f), clamping to that value.",
				builder.SamplerCreateInfo.maxAnisotropy, maxAniso);
			builder.SamplerCreateInfo.maxAnisotropy = maxAniso;
		}
		m_sampler = device->createSamplerUnique(builder.SamplerCreateInfo);
	}


	void VulkanTexture::UpdateDescriptorInfo()
	{
		m_descriptorInfo = vk::DescriptorImageInfo{
	m_sampler.get(), m_imageView.get(), m_layout
		};

	}


	void VulkanTexture::TransitionLayout(MyVkDevice& device, vk::ImageLayout newLayout, vk::CommandBuffer* commandBuffer)
	{
		MOE_ASSERT(m_image.get());

		// If a pre-existing command buffer is supplied, use this one - otherwise, just start an immediate command submit.
		if (commandBuffer != nullptr)
		{
			TransitionLayoutImpl(newLayout, *commandBuffer);
		}
		else
		{
			device.ImmediateCommandSubmit([&](vk::CommandBuffer immediateCmds)
				{
					TransitionLayoutImpl(newLayout, immediateCmds);
				});
		}

		m_layout = newLayout;
	}


	void VulkanTexture::GenerateMipmaps(MyVkDevice& device, vk::CommandBuffer commandBuffer, vk::ImageTiling tilingMode)
	{
		MOE_ASSERT(m_mipLevels > 0);
		if (m_mipLevels == 1)
			return; // nothing to do

		// It is very convenient to use vkCmdBlitImage to generate mipmaps, but unfortunately it is not guaranteed to be supported on all platforms.
		// It requires the texture image format we use to support linear filtering.
		const bool canBlit = device.TextureAllocator().HasTilingFeatureFor(m_format, tilingMode, vk::FormatFeatureFlagBits::eSampledImageFilterLinear);
		if (!canBlit)
		{
			MOE_WARNING(moe::ChanGraphics, "Blitting is unavailable for texture format %s, tiling %s, aborting mipmap generation",
				vk::to_string(m_format), vk::to_string(tilingMode));
			return;
		}

		if (!(m_usage & vk::ImageUsageFlagBits::eSampled))
		{
			MOE_WARNING(moe::ChanGraphics, "Generating mipmaps for a not-to-be-sampled image, probably not what you want!");
		}

		// Reusing those structures in the mip generation loop, setting some immutable data beforehand.
		int32_t mipWidth = m_dimensions.width;
		int32_t mipHeight = m_dimensions.height;

		vk::ImageMemoryBarrier imgBarrier{};
		imgBarrier.image = m_image.get();
		imgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imgBarrier.subresourceRange.aspectMask = FindImageAspect(m_usage, m_format);
		imgBarrier.subresourceRange.baseArrayLayer = 0;
		imgBarrier.subresourceRange.layerCount = m_layerCount;
		imgBarrier.subresourceRange.baseMipLevel = 0;
		imgBarrier.subresourceRange.levelCount = 1;

		vk::ImageBlit blit{};
		blit.srcSubresource.aspectMask = imgBarrier.subresourceRange.aspectMask;
		blit.srcSubresource.layerCount = m_layerCount;
		blit.dstSubresource.aspectMask = imgBarrier.subresourceRange.aspectMask;
		blit.dstSubresource.layerCount = m_layerCount;

		if (m_layout != vk::ImageLayout::eTransferDstOptimal)
		{
			// Make a first transition for all mips to be in the right layout (eTransferDstOptimal)
			imgBarrier.subresourceRange.baseMipLevel = 0;
			imgBarrier.subresourceRange.levelCount = m_mipLevels;
			imgBarrier.oldLayout = m_layout;
			imgBarrier.newLayout = vk::ImageLayout::eTransferDstOptimal;

			commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer,
				vk::DependencyFlags{}, 0, nullptr, 0, nullptr, 1, &imgBarrier);

			imgBarrier.subresourceRange.levelCount = 1; // alter only one mip at a time again.
		}

		for (int iMip = 1; iMip < (int)m_mipLevels; iMip++)
		{
			// First, we transition level i - 1 to transfer source optimal layout.
			// This transition will wait for level i - 1 to be filled, either from the previous blit command, or from vkCmdCopyBufferToImage.
			// The current blit command will wait on this transition.
			imgBarrier.subresourceRange.baseMipLevel = iMip - 1;
			imgBarrier.oldLayout =  vk::ImageLayout::eTransferDstOptimal; // mip0 is actually in shader read only layout but should be OK
			imgBarrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
			imgBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			imgBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

			commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer,
				vk::DependencyFlags{}, 0, nullptr, 0, nullptr, 1, &imgBarrier);

			// Next, we specify the regions that will be used in the blit operation.
			// The source mip level is i - 1 and the destination mip level is i.
			// The X and Y dimensions of the dstOffsets[1] are divided by two since each mip level is half the size of the previous level.
			blit.srcSubresource.mipLevel = iMip - 1;
			blit.srcOffsets[1] = vk::Offset3D{ mipWidth >> (iMip - 1), mipHeight >> (iMip - 1), 1 };

			blit.dstSubresource.mipLevel = iMip;
			blit.dstOffsets[1] = vk::Offset3D{ mipWidth >> iMip, mipHeight >> iMip, 1 };

			commandBuffer.blitImage(m_image.get(), vk::ImageLayout::eTransferSrcOptimal,
				m_image.get(), vk::ImageLayout::eTransferDstOptimal,
				1, &blit, vk::Filter::eLinear); // enable linear filter for interpolation

			// This barrier transitions mip level i - 1 to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL.
			// This transition waits on the current blit command to finish.
			// All sampling operations will wait on this transition to finish.
			imgBarrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
			imgBarrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			imgBarrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
			imgBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlags{},
				0, nullptr, 0, nullptr, 1, &imgBarrier);
		}

		// This barrier transitions the last mip level as it wasn't handled by the loop, since it is never blitted from.
		imgBarrier.subresourceRange.baseMipLevel = m_mipLevels - 1;
		imgBarrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
		imgBarrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		imgBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		imgBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlags{},
			0, nullptr, 0, nullptr, 1, &imgBarrier);
	}


	uint32_t VulkanTexture::ComputeNumberOfMipmapsForDimensions(uint32_t width, uint32_t height)
	{
		auto mipNbr = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
		return mipNbr;
	}


	vk::ImageAspectFlags VulkanTexture::FindImageAspect(vk::ImageUsageFlags usage, vk::Format format)
	{
		vk::ImageAspectFlags aspect{};

		if (usage & vk::ImageUsageFlagBits::eSampled)
		{
			aspect |= vk::ImageAspectFlagBits::eColor;
		}

		if (usage & vk::ImageUsageFlagBits::eColorAttachment)
		{
			aspect |= vk::ImageAspectFlagBits::eColor;
		}

		if (usage & vk::ImageUsageFlagBits::eDepthStencilAttachment)
		{
			aspect |= vk::ImageAspectFlagBits::eDepth;
		}

		if (FormatHasStencil(format))
		{
			aspect |= vk::ImageAspectFlagBits::eStencil;
		}

		return aspect;
	}


	bool VulkanTexture::FormatHasStencil(vk::Format format)
	{
		return (
			format == vk::Format::eD16UnormS8Uint ||
			format == vk::Format::eD24UnormS8Uint ||
			format == vk::Format::eD32SfloatS8Uint
			);
	}
}


#endif // MOE_VULKAN
