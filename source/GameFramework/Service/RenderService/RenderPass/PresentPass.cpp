#include "PresentPass.h"

#include "../Renderer/Renderer.h"
#include "Graphics/RHI/RenderHardwareInterface.h"

namespace moe
{
	PresentRenderPass::PresentRenderPass(Renderer& owner) :
		m_ownerRenderer(&owner)
	{
		auto* RHI = owner.MutRHI();
		m_swapchain = RHI->SwapchainManager().CreateSwapchain(RHI, owner.MutSurface());
	}


	void PresentRenderPass::Update()
	{
		m_ownerRenderer->MutRHI()->SwapchainManager().Present(m_swapchain);
	}
}
