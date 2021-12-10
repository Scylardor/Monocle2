// Monocle source files - Alexandre Baron

#pragma once
#include "RenderPass.h"
#include "Graphics/RHI/BufferManager/BufferManager.h"

#include "Graphics/RHI/FramebufferManager/FramebufferManager.h"
#include "Graphics/RHI/MaterialManager/MaterialManager.h"
#include "Graphics/RHI/SwapchainManager/SwapchainManager.h"

namespace moe
{
	class Renderer;

	class PresentRenderPass : public IRenderPass
	{
	public:
		PresentRenderPass(Renderer& owner, DeviceSwapchainHandle attachedSwapchain);

		~PresentRenderPass() override = default;

		void	Update(RenderQueue& drawQueue, uint8_t passIndex) override;

	private:

		Renderer*				m_ownerRenderer{ nullptr };

		DeviceFramebufferHandle	m_framebuffer{};

		DeviceSwapchainHandle	m_swapchain{};

		RenderMeshHandle		m_fullscreenQuadMesh{};

		DeviceMaterialHandle	m_fullscreenQuadMaterial{};
	};



}
