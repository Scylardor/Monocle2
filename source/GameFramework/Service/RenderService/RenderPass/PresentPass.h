// Monocle source files - Alexandre Baron

#pragma once
#include "RenderPass.h"
#include "Graphics/Handle/DeviceHandles.h"
#include "Graphics/RHI/FramebufferManager/FramebufferManager.h"
#include "Graphics/RHI/SwapchainManager/SwapchainManager.h"

namespace moe
{
	class Renderer;

	class PresentRenderPass : public IRenderPass
	{
	public:
		PresentRenderPass(Renderer& owner, DeviceSwapchainHandle attachedSwapchain);

		~PresentRenderPass() override = default;

		RenderQueueKey	Update(RenderQueue& drawQueue, RenderQueueKey key) override;

	private:

		Renderer*				m_ownerRenderer{ nullptr };

		DeviceSwapchainHandle	m_swapchain{};

		DeviceMeshHandle		m_fullscreenQuadMesh{};

		DeviceMaterialHandle	m_fullscreenQuadMaterial{};
	};



}
