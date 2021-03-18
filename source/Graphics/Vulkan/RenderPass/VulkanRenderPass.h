#pragma once
#include "Graphics/Vulkan/Framebuffer/FramebufferFactory.h"
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

namespace moe
{
	class MyVkDevice;
	class VulkanSwapchain;

	class VulkanRenderPass
	{
	public:

		VulkanRenderPass(VulkanRenderPass&& other);

		bool	Initialize(const MyVkDevice& device, const VulkanSwapchain& swapChain, FramebufferFactory::FramebufferID boundFramebuffer, VkRect2D renderArea, const std::array<vk::ClearValue, 2>& clearValues);

		void	Begin(vk::CommandBuffer cb);

		void	End(vk::CommandBuffer cb);


		operator vk::RenderPass() const
		{
			MOE_ASSERT(m_renderPass.get());
			return m_renderPass.get();
		}


		static VulkanRenderPass	New(const MyVkDevice& device, vk::Format colorAttachmentFormat);

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

		FramebufferFactory::FramebufferID	m_boundFramebuffer;	// the framebuffer this renderpass will use at render time.

		vk::RenderPassBeginInfo				m_commandBeginInfo;		// the info to pass to vkCmdBeginRenderPass.
		std::array<vk::ClearValue, 2>		m_commandClearValues{}; // We need to keep this with same lifetime as render pass as clear values is a pointer.
	};
}


#endif // MOE_VULKAN
