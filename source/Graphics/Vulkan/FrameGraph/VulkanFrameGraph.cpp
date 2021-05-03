
#ifdef MOE_VULKAN

#include "VulkanFrameGraph.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"
#include "Graphics/Vulkan/Swapchain/VulkanSwapchain.h"
#include "Graphics/Drawable/Drawable.h"
#include "Graphics/Vulkan/RenderPass/VulkanRenderPassBuilder.h"
#include "Graphics/Vulkan/Framebuffer/FramebufferFactory.h"


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


	bool VulkanFrameGraph::CreateMainRenderPass(MyVkDevice& device, VulkanSwapchain& swapChain)
	{
		(void)device;
		VulkanRenderPassBuilder builder;
		VulkanRenderPass rp = builder.AttachPresentImage(FramebufferFactory::SWAPCHAIN_COLOR_ATTACHMENT_VIEW,
			swapChain.GetColorAttachmentFormat())
			.AttachDepthStencilImage(FramebufferFactory::SWAPCHAIN_DEPTH_STENCIL_ATTACHMENT_VIEW,
				swapChain.GetDepthAttachmentFormat(), swapChain.GetNumSamples(), vk::ClearDepthStencilValue(1.0f, 0))
			.AttachColorImage(FramebufferFactory::SWAPCHAIN_MULTISAMPLE_ATTACHMENT_VIEW, swapChain.GetColorAttachmentFormat(), swapChain.GetNumSamples())
			.AddSubPass()
				.AttachColorReference(2, 0)
				.AttachDepthStencilReference(1)
			.AddDependency(VK_SUBPASS_EXTERNAL, 0,
				vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
				vk::AccessFlagBits(),
				vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
				vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite)
			.Build(swapChain);

			ConnectRenderPass(std::move(rp));

		return true;
	}
}


#endif // MOE_VULKAN
