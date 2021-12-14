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

		void	Update(RenderQueue& drawQueue, uint8_t passIndex) override;

	private:

		void	CreateFramebuffer(std::pair<int, int> const& dimensions);

		void	OnGraphicsSurfaceResized(int newWidth, int newHeight);


		Renderer*				m_ownerRenderer{ nullptr };

		EventDelegateID			m_surfaceResizeDelID{};

		DeviceFramebufferHandle	m_framebuffer{};

	};



}
