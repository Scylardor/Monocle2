#pragma once
#include "Graphics/Vulkan/Framebuffer/FramebufferFactory.h"
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

#include "VulkanRenderPassBuilder.h"

namespace moe
{
	class MyVkDevice;
	class VulkanSwapchain;

	class VulkanRenderPass
	{
	public:

		VulkanRenderPass() = default;

		VulkanRenderPass(VulkanSwapchain& swapChain, VulkanRenderPassBuilder& builder);

		VulkanRenderPass(VulkanRenderPass&& other);

		bool	Initialize(const VulkanSwapchain& swapChain, VulkanRenderPassBuilder& builder);

		void	CreateFramebuffer(const VulkanSwapchain& swapChain, VulkanRenderPassBuilder& builder);


		void	Begin(vk::CommandBuffer cb, uint32_t framebufferIndex = 0);

		void	End(vk::CommandBuffer cb);


		operator vk::RenderPass() const
		{
			MOE_ASSERT(m_renderPass.get());
			return m_renderPass.get();
		}


		static VulkanRenderPass	New(const MyVkDevice& device, vk::Format colorAttachmentFormat, vk::Format depthStencilAttachmentFormat, vk::SampleCountFlagBits numSamples = vk::SampleCountFlagBits::e1);

	protected:

		VulkanRenderPass(vk::UniqueRenderPass&& renderPass)
		{
			m_renderPass = std::move(renderPass);
		}

	private:

		const MyVkDevice&	Device() const
		{
			MOE_ASSERT(m_device != nullptr);
			return *m_device;
		}

		void	InitializeRenderPass(vk::Format attachmentFormat);

		const MyVkDevice* m_device = nullptr;

		vk::UniqueRenderPass				m_renderPass;		// the render pass.

		vk::RenderPassBeginInfo				m_commandBeginInfo;		// the info to pass to vkCmdBeginRenderPass.

		std::vector<vk::UniqueFramebuffer>	m_framebuffers{1};

		VulkanRenderPassBuilder				m_buildInfos{};
	};
}


#endif // MOE_VULKAN
