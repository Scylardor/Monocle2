
#ifdef MOE_VULKAN

#include "VulkanRenderPassBuilder.h"


#include "Graphics/Vulkan/RenderPass/VulkanRenderPass.h"
#include "Graphics/Vulkan/Swapchain/VulkanSwapchain.h"
#include "Graphics/Vulkan/Texture/VulkanTexture.h"


moe::VulkanRenderPassBuilder& moe::VulkanRenderPassBuilder::AddAttachment(vk::ImageView colorView,
	vk::Format format, vk::SampleCountFlagBits samples,
	vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeop,
	vk::AttachmentLoadOp stencilLoadOp, vk::AttachmentStoreOp stencilStoreOp,
	vk::ImageLayout initialLayout, vk::ImageLayout finalLayout)
{
	AttachmentsDescriptions.emplace_back(vk::AttachmentDescriptionFlags(),
		format,
		samples,
		loadOp,
		storeop,
		stencilLoadOp,
		stencilStoreOp,
		initialLayout,
		finalLayout);

	AttachmentViews.push_back(colorView);
	return *this;
}


moe::VulkanRenderPassBuilder& moe::VulkanRenderPassBuilder::AttachPresentImage(vk::ImageView colorView, vk::Format format, vk::ClearColorValue clearColorValue)
{
	ClearValues.emplace_back(clearColorValue);

	return AddAttachment(colorView, format, vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
		vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
		vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);

}


moe::VulkanRenderPassBuilder& moe::VulkanRenderPassBuilder::AttachDepthStencilImage(vk::ImageView depthView, vk::Format format, vk::SampleCountFlagBits samples, vk::ClearDepthStencilValue clearDSValue)
{
	ClearValues.emplace_back(clearDSValue);

	const vk::AttachmentLoadOp stencilLoadOp = (VulkanTexture::FormatHasStencil(format) ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eDontCare);

	return AddAttachment(depthView, format, samples,
		vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eDontCare, // The depth-stencil buffer is "one-use only" so we can trash the results afterwards
		stencilLoadOp, vk::AttachmentStoreOp::eDontCare, // same as for depth.
		vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
}


moe::VulkanRenderPassBuilder& moe::VulkanRenderPassBuilder::AttachColorImage(vk::ImageView colorView, vk::Format format, vk::SampleCountFlagBits samples, vk::ClearColorValue clearColorValue)
{
	ClearValues.emplace_back(clearColorValue);

	return AddAttachment(colorView, format, samples,
		vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
		vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
		vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal);
}


moe::VulkanRenderPassBuilder& moe::VulkanRenderPassBuilder::AddSubPass()
{
	SubpassDescriptions.emplace_back();
	SubpassReferences.emplace_back();
	return *this;
}


moe::VulkanRenderPassBuilder& moe::VulkanRenderPassBuilder::AttachColorReference(uint32_t attachment, uint32_t resolveAttachment, vk::ImageLayout layout, uint32_t subpass)
{
	if (subpass == LAST_SUBPASS)
		subpass = (uint32_t)SubpassDescriptions.size() - 1;

	ResizeForSubpass(subpass);

	SubpassReferences[subpass].ColorAttachmentRefs.emplace_back(attachment, layout);

	if (resolveAttachment != NO_RESOLVE)
		SubpassReferences[subpass].ResolveRefs.emplace_back(resolveAttachment, layout);

	SubpassDescriptions[subpass].colorAttachmentCount++;

	return *this;
}


moe::VulkanRenderPassBuilder& moe::VulkanRenderPassBuilder::AttachDepthStencilReference(uint32_t attachment, uint32_t subpass)
{
	if (subpass == LAST_SUBPASS)
		subpass = (uint32_t)SubpassDescriptions.size() - 1;

	ResizeForSubpass(subpass);

	SubpassReferences[subpass].DepthStencilRef = vk::AttachmentReference{ attachment, vk::ImageLayout::eDepthStencilAttachmentOptimal };
	SubpassDescriptions[subpass].pDepthStencilAttachment = &SubpassReferences[subpass].DepthStencilRef;
	return *this;
}


moe::VulkanRenderPassBuilder& moe::VulkanRenderPassBuilder::AddDependency(uint32_t from, uint32_t to,
	vk::PipelineStageFlags srcStage, vk::AccessFlags srcAccessFlags, vk::PipelineStageFlags dstStage,
	vk::AccessFlags dstAccessFlags)
{
	SubpassDependencies.emplace_back(from, to, srcStage, dstStage, srcAccessFlags, dstAccessFlags);
	return *this;
}


moe::VulkanRenderPassBuilder& moe::VulkanRenderPassBuilder::AttachPreserveReference(uint32_t attachment,
																					uint32_t subpass)
{
	if (subpass == LAST_SUBPASS)
		subpass = (uint32_t)SubpassDescriptions.size() - 1;

	ResizeForSubpass(subpass);

	SubpassReferences[subpass].PreserveRefs.emplace_back(attachment);
	return *this;
}





moe::VulkanRenderPass moe::VulkanRenderPassBuilder::Build(VulkanSwapchain& swapChain)
{
	// some sanity checks
	MOE_ASSERT(ClearValues.size() == AttachmentViews.size() && ClearValues.size() == AttachmentsDescriptions.size());

	for (auto iSP = 0u; iSP < SubpassReferences.size(); iSP++)
	{
		auto& thisSubpassDesc = SubpassDescriptions[iSP];
		const auto& thisSubpassRefs = SubpassReferences[iSP];

		MOE_ASSERT(thisSubpassRefs.ResolveRefs.empty() || thisSubpassRefs.ResolveRefs.size() == thisSubpassRefs.ColorAttachmentRefs.size());

		thisSubpassDesc.colorAttachmentCount	= (uint32_t)thisSubpassRefs.ColorAttachmentRefs.size();
		thisSubpassDesc.pColorAttachments		= thisSubpassRefs.ColorAttachmentRefs.data();
		thisSubpassDesc.pDepthStencilAttachment	= &thisSubpassRefs.DepthStencilRef;
		thisSubpassDesc.pResolveAttachments		= thisSubpassRefs.ResolveRefs.data();
		thisSubpassDesc.preserveAttachmentCount	= (uint32_t)thisSubpassRefs.PreserveRefs.size();
		thisSubpassDesc.pPreserveAttachments	= thisSubpassRefs.PreserveRefs.data();
		thisSubpassDesc.inputAttachmentCount	= (uint32_t)thisSubpassRefs.InputRefs.size();
		thisSubpassDesc.pInputAttachments		= thisSubpassRefs.InputRefs.data();
	}

	return VulkanRenderPass(swapChain, *this);
}


void moe::VulkanRenderPassBuilder::ResizeForSubpass(uint32_t subpass)
{
	MOE_ASSERT(SubpassDescriptions.size() == SubpassReferences.size());

	if (subpass >= SubpassDescriptions.size())
	{
		SubpassDescriptions.resize(subpass + 1);
		SubpassReferences.resize(subpass + 1);
	}
}

#endif // MOE_VULKAN
