#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

#include <vector>

namespace moe
{
	class VulkanSwapchain;
	class VulkanTexture;



	struct VulkanRenderPassBuilder
	{
		struct SubpassReferences
		{
			std::vector<vk::AttachmentReference>	ColorAttachmentRefs{};
			vk::AttachmentReference					DepthStencilRef{};
			std::vector<vk::AttachmentReference>	ResolveRefs{};
			std::vector<vk::AttachmentReference>	InputRefs{};
			std::vector<uint32_t>					PreserveRefs{};
		};


	public:

		// Use as a "default" for AddReference etc. to affect the last added subpass (avoids to specify an index)
		static const auto LAST_SUBPASS = (~0u);

		// Use as a magic value for "no resolve attachment associated" with this color target when attaching references
		static const auto NO_RESOLVE = (~0u);

		typedef std::vector<vk::ImageView>							AttachmentViewsArray;
		typedef std::vector<vk::AttachmentDescription>				AttachmentsArray;
		typedef std::vector<SubpassReferences>						SubpassReferencesArray;
		typedef std::vector<uint32_t>								PreservedAttachmentsArray;
		typedef std::vector<vk::SubpassDescription>					SubpassDescriptionsArray;
		typedef std::vector<vk::SubpassDependency>					SubpassDependenciesArray;

		VulkanRenderPassBuilder() = default;
		VulkanRenderPassBuilder(const VulkanRenderPassBuilder&) = delete;

		VulkanRenderPassBuilder&	AddAttachment(vk::ImageView colorView,
			vk::Format format, vk::SampleCountFlagBits samples,
			vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeop,
			vk::AttachmentLoadOp stencilLoadOp, vk::AttachmentStoreOp stencilStoreOp,
			vk::ImageLayout initialLayout, vk::ImageLayout finalLayout);

		VulkanRenderPassBuilder&	AttachPresentImage(vk::ImageView colorView, vk::Format format, vk::ClearColorValue clearColorValue = vk::ClearColorValue());

		VulkanRenderPassBuilder&	AttachDepthStencilImage(vk::ImageView depthView, vk::Format format, vk::SampleCountFlagBits samples, vk::ClearDepthStencilValue clearDSValue = vk::ClearDepthStencilValue());

		VulkanRenderPassBuilder&	AttachColorImage(vk::ImageView colorView, vk::Format format, vk::SampleCountFlagBits samples, vk::ClearColorValue clearColorValue = vk::ClearColorValue());

		VulkanRenderPassBuilder&	AttachColorReference(uint32_t attachment, uint32_t resolveAttachment = NO_RESOLVE, vk::ImageLayout layout = vk::ImageLayout::eColorAttachmentOptimal, uint32_t subpass = LAST_SUBPASS);

		VulkanRenderPassBuilder&	AttachPreserveReference(uint32_t attachment, uint32_t subpass = LAST_SUBPASS);

		VulkanRenderPassBuilder&	AttachInputReference(uint32_t attachment, uint32_t subpass = LAST_SUBPASS);

		VulkanRenderPassBuilder&	AttachDepthStencilReference(uint32_t attachment, uint32_t subpass = LAST_SUBPASS);

		VulkanRenderPassBuilder&	AddSubPass();

		VulkanRenderPassBuilder&	AddDependency(uint32_t from, uint32_t to,
			vk::PipelineStageFlags srcStage, vk::AccessFlags srcAccessFlags,
			vk::PipelineStageFlags dstStage, vk::AccessFlags dstAccessFlags);

		VulkanRenderPassBuilder&	SetRenderArea(VkRect2D renderArea)
		{
			RenderArea = renderArea;
			return *this;
		}


		class VulkanRenderPass		Build(VulkanSwapchain& swapChain);


		AttachmentViewsArray		AttachmentViews{};
		AttachmentsArray			AttachmentsDescriptions{};
		SubpassDescriptionsArray	SubpassDescriptions{};
		SubpassReferencesArray		SubpassReferences{};
		SubpassDependenciesArray	SubpassDependencies{};
		PreservedAttachmentsArray	PreservedAttachments{};

		VkRect2D					RenderArea{ };
		std::vector<vk::ClearValue>	ClearValues;

	private:

		void	ResizeForSubpass(uint32_t subpass);

	};
}


#endif // MOE_VULKAN
