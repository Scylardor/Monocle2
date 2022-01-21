// Monocle source files - Alexandre Baron

#pragma once
#include "RenderPass.h"
#include "Core/Delegates/EventDelegateID.h"
#include "Graphics/RHI/FramebufferManager/FramebufferManager.h"

namespace moe
{
	class Renderer;

	class GeometryRenderPass : public ISubpass
	{
	public:
		GeometryRenderPass(Renderer& owner);

		~GeometryRenderPass() override;

		RenderQueueKey	Update(RenderQueue& drawQueue, RenderQueueKey key, int subpassIdx) override;

	private:


		Renderer*				m_ownerRenderer{ nullptr };

		EventDelegateID			m_surfaceResizeDelID{};

		DeviceFramebufferHandle	m_framebuffer{};

	};



}
