#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

#include "Graphics/Vulkan/Framebuffer/FramebufferFactory.h"
#include "Graphics/Vulkan/RenderPass/VulkanRenderPass.h"
#include "Graphics/Vulkan/CommandPipeline/VulkanCommandPipeline.h"

namespace moe
{
	class DrawableWorld;


	class VulkanFrameGraph
	{
	public:

		typedef	std::vector<VulkanRenderPass>	RenderPassList;

		bool	CreateMainRenderPass(class MyVkDevice& device, VulkanSwapchain& swapChain);

		void	ConnectRenderPass(VulkanRenderPass&& renderPass);

		void	Render(const MyVkDevice& device, const DrawableWorld& drawables);

		RenderPassList::iterator	begin()
		{
			return m_renderPasses.begin();
		}

		RenderPassList::const_iterator	begin() const
		{
			return m_renderPasses.begin();
		}

		RenderPassList::iterator end()
		{
			return m_renderPasses.end();
		}

		RenderPassList::const_iterator end() const
		{
			return m_renderPasses.end();
		}

		const VulkanRenderPass&	MainRenderPass() const
		{
			MOE_ASSERT(false == m_renderPasses.empty());
			return m_renderPasses.front();
		}

		VulkanRenderPass& MutMainRenderPass()
		{
			MOE_ASSERT(false == m_renderPasses.empty());
			return m_renderPasses.front();
		}


	protected:




	private:
		RenderPassList			m_renderPasses;

		VulkanCommandPipeline	m_pipeline;
	};

}


#endif // MOE_VULKAN
