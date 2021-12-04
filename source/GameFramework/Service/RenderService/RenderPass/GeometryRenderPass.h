// Monocle source files - Alexandre Baron

#pragma once
#include "RenderPass.h"
#include "Core/Delegates/EventDelegateID.h"
#include "Graphics/RHI/FramebufferManager/FramebufferManager.h"
#include "Graphics/RHI/SwapchainManager/SwapchainManager.h"

namespace moe
{
	class Renderer;

	class GeometryRenderPass : public IRenderPass
	{
	public:
		GeometryRenderPass(Renderer& owner);

		~GeometryRenderPass() override;

		void	Update() override;

	private:

		void	OnGraphicsSurfaceResized(int newWidth, int newHeight);


		Renderer*				m_ownerRenderer{ nullptr };

		EventDelegateID			m_surfaceResizeDelID{};

		DeviceFramebufferHandle	m_framebuffer{};

		DeviceSwapchainHandle	m_swapchain{};
	};



}
