// Monocle source files - Alexandre Baron

#pragma once
#include "RenderPass.h"

#include "Graphics/RHI/FramebufferManager/FramebufferManager.h"
#include "Graphics/RHI/SwapchainManager/SwapchainManager.h"

namespace moe
{
	class Renderer;

	class PresentRenderPass : public IRenderPass
	{
	public:
		PresentRenderPass(Renderer& owner);

		~PresentRenderPass() override = default;

		void	Update() override;

	private:

		Renderer*				m_ownerRenderer{ nullptr };

		DeviceFramebufferHandle	m_framebuffer{};

		DeviceSwapchainHandle	m_swapchain{};
	};



}
