#ifdef MOE_VULKAN

#include "VulkanRenderPass.h"
#include "Graphics/Vulkan/Device/VulkanDevice.h"
#include "Graphics/Vulkan/Swapchain/VulkanSwapchain.h"

#include "Graphics/Vulkan/RenderPass/VulkanRenderPassBuilder.h"


namespace moe
{
	VulkanRenderPass::VulkanRenderPass(VulkanSwapchain& swapChain, VulkanRenderPassBuilder& builder)
	{
		vk::RenderPassCreateInfo renderPassInfo{};
		renderPassInfo.attachmentCount = (uint32_t)builder.AttachmentsDescriptions.size();
		renderPassInfo.pAttachments = builder.AttachmentsDescriptions.data();
		renderPassInfo.subpassCount = (uint32_t)builder.SubpassDescriptions.size();
		renderPassInfo.pSubpasses = builder.SubpassDescriptions.data();
		renderPassInfo.dependencyCount = (uint32_t)builder.SubpassDependencies.size();
		renderPassInfo.pDependencies = builder.SubpassDependencies.data();

		m_renderPass = swapChain.Device()->createRenderPassUnique(renderPassInfo);

		m_buildInfos = builder;

		bool ok = Initialize(swapChain, m_buildInfos);
		MOE_ASSERT(ok);


	}

	VulkanRenderPass::VulkanRenderPass(VulkanRenderPass&& other)
	{
		m_device = other.m_device;
		m_renderPass = std::move(other.m_renderPass);
		m_buildInfos = std::move(other.m_buildInfos);
		m_commandBeginInfo = other.m_commandBeginInfo;
		m_commandBeginInfo.pClearValues = m_buildInfos.ClearValues.data();
		m_framebuffers = std::move(other.m_framebuffers);

	}

	bool VulkanRenderPass::Initialize(const VulkanSwapchain& swapChain, VulkanRenderPassBuilder& builder)
	{
		MOE_ASSERT(m_device == nullptr);
		m_device = &swapChain.Device();

		m_commandBeginInfo.renderPass = m_renderPass.get();
		m_commandBeginInfo.framebuffer = vk::Framebuffer(); // will be fetched just in time by Begin().

		// Automatically replace extent magic values by the actual swap chain extent
		m_commandBeginInfo.renderArea = builder.RenderArea;
		if (m_commandBeginInfo.renderArea.extent.width == FramebufferFactory::SWAPCHAIN_FRAMEBUFFER_WIDTH)
			m_commandBeginInfo.renderArea.extent.width = swapChain.GetSwapchainImageExtent().width;

		if (m_commandBeginInfo.renderArea.extent.height == FramebufferFactory::SWAPCHAIN_FRAMEBUFFER_HEIGHT)
			m_commandBeginInfo.renderArea.extent.height = swapChain.GetSwapchainImageExtent().height;

		m_commandBeginInfo.clearValueCount = (uint32_t)builder.ClearValues.size();
		m_commandBeginInfo.pClearValues = builder.ClearValues.data();

		CreateFramebuffer(swapChain, builder);


		return true;
	}


	void VulkanRenderPass::CreateFramebuffer(const VulkanSwapchain& swapChain, VulkanRenderPassBuilder& builder)
	{
		m_framebuffers.clear();

		// Make a copy of the views array because we might modify it.
		// Scan for swapchain color attachment view in the attached image views.
		// If there is, we have to create as many framebuffers as there are maximum images in flight possible.
		VulkanRenderPassBuilder::AttachmentViewsArray views = builder.AttachmentViews;

		std::vector< VulkanRenderPassBuilder::AttachmentViewsArray > framebufferViews;
		framebufferViews.reserve(swapChain.GetImagesInFlightCount());

		bool needsMultiFramebuffers = false;

		for (auto iSwapImg = 0u; iSwapImg < swapChain.GetImagesInFlightCount(); iSwapImg++)
		{
			auto& fbViews = framebufferViews.emplace_back(builder.AttachmentViews);
			for (vk::ImageView& view : fbViews)
			{
				if (view == FramebufferFactory::SWAPCHAIN_COLOR_ATTACHMENT_VIEW)
				{
					view = swapChain.GetColorAttachmentView(iSwapImg);
					needsMultiFramebuffers = true;
				}
				else if (view == FramebufferFactory::SWAPCHAIN_DEPTH_STENCIL_ATTACHMENT_VIEW)
					view = swapChain.GetDepthAttachmentView();
				else if (view == FramebufferFactory::SWAPCHAIN_MULTISAMPLE_ATTACHMENT_VIEW)
					view = swapChain.GetMultisampleAttachmentView();
			}

			if (needsMultiFramebuffers == false) // no swap chain color attachment detected: we only need one framebuffer.
				break;
		}

		// Use images in flight count (eg. : 10) instead of max frames in flight (eg: 2 or 3), why?
		// Because mailbox present for example might return us images 0 1 2 9 9 8 9 8, but using only max frames in flight,
		// we only create framebuffers for the 0 and 1 images if it's equal to 2 for example.
		auto nbReservedFb = (needsMultiFramebuffers ? swapChain.GetImagesInFlightCount() : 1);
		m_framebuffers.reserve(nbReservedFb);

		vk::FramebufferCreateInfo framebufferInfo{};
		framebufferInfo.renderPass = m_renderPass.get();
		framebufferInfo.attachmentCount = (uint32_t) builder.AttachmentViews.size();
		framebufferInfo.width = m_commandBeginInfo.renderArea.extent.width;
		framebufferInfo.height = m_commandBeginInfo.renderArea.extent.height;
		framebufferInfo.layers = 1;

		MOE_ASSERT(nbReservedFb == 1 || needsMultiFramebuffers);

		for (auto iFb = 0u; iFb < nbReservedFb; iFb++)
		{
			framebufferInfo.pAttachments = framebufferViews[iFb].data();
			m_framebuffers.emplace_back(swapChain.Device()->createFramebufferUnique(framebufferInfo));
		}
	}


	void VulkanRenderPass::Begin(vk::CommandBuffer cb, uint32_t framebufferIndex)
	{
		MOE_ASSERT(framebufferIndex < m_framebuffers.size());

		// Fetch just-in-time the framebuffer to use for this frame

		m_commandBeginInfo.framebuffer = m_framebuffers[framebufferIndex].get();
		MOE_ASSERT(m_commandBeginInfo.framebuffer);

		// The render pass commands will be embedded in the primary command buffer itself and no secondary command buffers will be executed.
		cb.beginRenderPass(&m_commandBeginInfo, vk::SubpassContents::eInline); // TODO: always inline for now, to be customized
	}

	void VulkanRenderPass::End(vk::CommandBuffer cb)
	{
		cb.endRenderPass();
	}

	// At the moment, a hardcoded "dummy function".
	VulkanRenderPass VulkanRenderPass::New(const MyVkDevice& device, vk::Format colorAttachmentFormat, vk::Format depthStencilAttachmentFormat, vk::SampleCountFlagBits numSamples)
	{
		std::array<vk::AttachmentDescription, 3> attachments{};

		vk::AttachmentDescription& colorAttachment = attachments[0];
		colorAttachment.format = colorAttachmentFormat;
		colorAttachment.samples = vk::SampleCountFlagBits::e1; // This is the "final" color resolve buffer so it will never be multisampled.
		colorAttachment.loadOp = vk::AttachmentLoadOp::eClear; // clear the framebuffer to black before drawing a new frame.
		colorAttachment.storeOp = vk::AttachmentStoreOp::eStore; // Rendered contents can be read later
		colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare; // it's a color attachment, so the results of loading and storing the stencil are irrelevant.
		colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachment.initialLayout = vk::ImageLayout::eUndefined; // doesn't matter since we're going to clear it anyway.
		colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR; // We want the image to be ready for presentation using the swap chain after rendering

		vk::AttachmentReference colorAttachmentRef;
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;


		vk::AttachmentDescription& depthAttachment = attachments[1];
		depthAttachment.format = depthStencilAttachmentFormat;
		depthAttachment.samples = numSamples;
		depthAttachment.loadOp = vk::AttachmentLoadOp::eClear; // clear the framebuffer before drawing a new frame.
		depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare; // The depth-stencil buffer is "one-use only" so we can trash the results afterwards
		if (VulkanTexture::FormatHasStencil(depthStencilAttachmentFormat)) // Only use the stencil buffer if the depth format includes a stencil component.
			depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eClear;
		else
			depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare; // same as for depth.
		depthAttachment.initialLayout = vk::ImageLayout::eUndefined; // doesn't matter since we're going to clear it anyway.
		depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal; // We want the image to be ready for depth test before rendering

		vk::AttachmentReference depthAttachmentRef;
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = depthAttachment.finalLayout;

		vk::AttachmentDescription& multisampleAttachment = attachments[2];
		// This is vastly similar to the regular color attachment...
		multisampleAttachment = colorAttachment;
		// ... except the samples count and final layout.
		multisampleAttachment.samples = numSamples;
		multisampleAttachment.finalLayout = vk::ImageLayout::eColorAttachmentOptimal; // This attachment won't be presented and will need to be resolved.

		vk::AttachmentReference multiSampleAttachmentRef;
		multiSampleAttachmentRef.attachment = 2;
		multiSampleAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;


		vk::SubpassDescription subpass{};
		// Vulkan may also support compute subpasses in the future, so we have to be explicit about this being a graphics subpass.
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = (numSamples == vk::SampleCountFlagBits::e1 ? &colorAttachmentRef : &multiSampleAttachmentRef);
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		subpass.pResolveAttachments = (numSamples == vk::SampleCountFlagBits::e1 ? nullptr : &colorAttachmentRef);

		// Create an explicit dependency on the subpass to make sure we acquire the image at the right time
		vk::SubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;

		// We need to wait for the swap chain to finish reading from the image before we can access it.
		// The depth image is first accessed in the early fragment test pipeline stage.
		dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
		dependency.srcAccessMask = vk::AccessFlagBits{};

		// The operations that should wait on this are in the color attachment stage and involve the writing of the color attachment.
		// And because we probably have a depth load operation that clears, we should specify the access mask for writes.
		dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
		dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

		vk::RenderPassCreateInfo renderPassInfo{};
		renderPassInfo.attachmentCount = (uint32_t) attachments.size();
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		return VulkanRenderPass(device->createRenderPassUnique(renderPassInfo));
	}


}


#endif // MOE_VULKAN
