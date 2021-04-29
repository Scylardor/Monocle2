#ifdef MOE_VULKAN

#include "VulkanFrameGraph.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"
#include "Graphics/Vulkan/Swapchain/VulkanSwapchain.h"
#include "Graphics/Drawable/Drawable.h"

namespace moe
{
	void VulkanFrameGraph::ConnectRenderPass(VulkanRenderPass&& renderPass)
	{
		m_renderPasses.emplace_back(std::move(renderPass));


	}


	void VulkanFrameGraph::Render(const MyVkDevice& device, const DrawableWorld& /*drawables*/)
	{
		m_pipeline.Reset(device);

		for (auto& renderPass : m_renderPasses)
		{
			auto vkCmdList = m_pipeline.GrabCommandBuffer(device);
			renderPass.Begin(vkCmdList);

			//for (const auto& drawable : drawables)
			//{
			//}

			renderPass.End(vkCmdList);
		}
	}


	bool VulkanFrameGraph::CreateMainRenderPass(MyVkDevice& device, const VulkanSwapchain& swapChain)
	{
		VulkanRenderPass rp = VulkanRenderPass::New(device, swapChain.GetColorAttachmentFormat(), swapChain.GetDepthAttachmentFormat(), swapChain.GetNumSamples());

		std::array<vk::ImageView, 3>  attachments = {
			FramebufferFactory::SWAPCHAIN_COLOR_ATTACHMENT_VIEW,
			FramebufferFactory::SWAPCHAIN_DEPTH_STENCIL_ATTACHMENT_VIEW,
			FramebufferFactory::SWAPCHAIN_MULTISAMPLE_ATTACHMENT_VIEW
		};

		auto attachmentCount = (uint32_t)attachments.size();
		if (false == swapChain.HasMultisampleAttachment())
			attachmentCount--;

		vk::FramebufferCreateInfo framebufferInfo{};
		framebufferInfo.renderPass = rp;
		framebufferInfo.attachmentCount = attachmentCount;
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = FramebufferFactory::SWAPCHAIN_FRAMEBUFFER_WIDTH;
		framebufferInfo.height = FramebufferFactory::SWAPCHAIN_FRAMEBUFFER_HEIGHT;
		framebufferInfo.layers = 1;
		FramebufferFactory::FramebufferID rpFramebuffer = device.FramebufferFactory.CreateFramebuffer(framebufferInfo);

		//  the clear values to use for VK_ATTACHMENT_LOAD_OP_CLEAR
		std::array<vk::ClearValue, 3> clearValues{};
		clearValues[0].color = std::array<float, 4>({ 0.0f, 0.0f, 0.5f, 1.0f });
		clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);
		clearValues[2] = clearValues[0];

		bool ok = rp.Initialize(device, swapChain, rpFramebuffer,
			{ {0, 0}, { FramebufferFactory::SWAPCHAIN_FRAMEBUFFER_WIDTH, FramebufferFactory::SWAPCHAIN_FRAMEBUFFER_HEIGHT} },
			clearValues);

		MOE_ASSERT(ok);
		if (ok)
			ConnectRenderPass(std::move(rp));

		return ok;
	}
}


#endif // MOE_VULKAN
