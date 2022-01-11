// Monocle source files - Alexandre Baron

#include "GeometryRenderPass.h"

#include "GameFramework/Service/RenderService/Renderer/Renderer.h"

#include "GameFramework/Service/RenderService/GraphicsSurface/GraphicsSurface.h"
#include "Graphics/RenderQueue/RenderQueue.h"
#include "GameFramework/Service/RenderService/RenderScene/RenderScene.h"

#include "Graphics/RHI/RenderHardwareInterface.h"

namespace moe
{
	GeometryRenderPass::GeometryRenderPass(Renderer& owner) :
		m_ownerRenderer(&owner)
	{
		auto* gfxSurface = m_ownerRenderer->MutSurface();
		MOE_DEBUG_ASSERT(gfxSurface != nullptr);
		auto dimensions = gfxSurface->GetDimensions();

		CreateFramebuffer(dimensions);

		DeviceSwapchainHandle mainSwapchainHandle{ 0 };
		m_ownerRenderer->MutRHI()->SwapchainManager().RegisterSwapchainResizedCallback(
			mainSwapchainHandle,
			{ [this](int width, int height)
			{
				this->OnGraphicsSurfaceResized(width, height);
			} }
		);
	}


	GeometryRenderPass::~GeometryRenderPass()
	{
	}


	void GeometryRenderPass::Update(RenderQueue& drawQueue, uint8_t passIndex)
	{
		RenderQueueKey key = RenderQueue::ComputeRenderQueueKey(passIndex);
		key = drawQueue.EmplaceCommand<CmdBeginRenderPass>(key, m_framebuffer, ColorRGBAf::Black());

		RenderScene & renderedScene = *m_ownerRenderer->MutAttachedScene();

		uint8_t viewportIdx = 0;
		for (ViewObject const& view : renderedScene.GetViews())
		{
			MOE_ASSERT(viewportIdx < RenderQueueKey::MAX_VIEWPORTS);
			key.ViewportID = viewportIdx;

			drawQueue.EmplaceDrawCall<CmdSetViewportScissor>(key, view.ScreenViewport(), view.ScreenScissor());

			DeviceMaterialHandle lastMaterialUsed = DeviceMaterialHandle::Null();
			std::pair<uint32_t, uint32_t> lastMaterialIDs{};

			for (RenderObject const& object : renderedScene)
			{
				auto objectMaterialHandle = object.GetMaterialHandle();

				if (objectMaterialHandle != lastMaterialUsed)
				{
					auto thisMaterialIDs = objectMaterialHandle.DecomposeMaterialAndShaderIndices();

					key.Material = thisMaterialIDs.first;
					key.Program = thisMaterialIDs.second;

					drawQueue.EmplaceDrawCall<CmdBindMaterial>(key, objectMaterialHandle);

					// If it's the same shader program, but with different uniforms, we don't need to rebind the scenes' resource sets.
					// If it is not, we have to rebind everything
					if (lastMaterialIDs.second != thisMaterialIDs.second)
					{
						renderedScene.OnRenderShaderChange().Broadcast(renderedScene, view, drawQueue, key);
					}


					lastMaterialUsed = objectMaterialHandle;
					lastMaterialIDs = thisMaterialIDs;
				}

				renderedScene.SetObjectTransform(object.GetTransformID(), view.GetViewProjectionMatrix() * object.GetModelMatrix());
				drawQueue.EmplaceDrawCall<CmdDrawMesh>(key, object.GetMeshHandle(), object.GetDynamicSetsHandle());
			}

			viewportIdx++;
		}

		drawQueue.EmplaceCommand<CmdEndRenderPass>(key);
	}


	void GeometryRenderPass::CreateFramebuffer(std::pair<int, int> const& dimensions)
	{
		auto* RHI = m_ownerRenderer->MutRHI();
		auto& framebufferManager = RHI->FramebufferManager();
		m_framebuffer = framebufferManager.CreateFramebuffer(dimensions);

		ISwapchainManager& scManager = RHI->SwapchainManager();
		DeviceTextureHandle scColorAttach = scManager.GetMainSwapchainColorAttachment();
		DeviceTextureHandle scDepthAttach = scManager.GetMainSwapchainDepthStencilAttachment();

		framebufferManager.AddColorAttachment(m_framebuffer, scColorAttach);
		framebufferManager.SetDepthStencilAttachment(m_framebuffer, scDepthAttach);
	}


	void GeometryRenderPass::OnGraphicsSurfaceResized(int newWidth, int newHeight)
	{
		auto* RHI = m_ownerRenderer->MutRHI();
		auto& framebufferManager = RHI->FramebufferManager();

		// dont destroy the attachments of the swap chain : the swap chain probably deleted them itself
		framebufferManager.DestroyFramebuffer(m_framebuffer, IFramebufferManager::DestroyAttachmentMode::KeepAttachments);

		m_framebuffer = framebufferManager.CreateFramebuffer({newWidth, newHeight});

		ISwapchainManager& scManager = RHI->SwapchainManager();
		DeviceTextureHandle scColorAttach = scManager.GetMainSwapchainColorAttachment();
		DeviceTextureHandle scDepthAttach = scManager.GetMainSwapchainDepthStencilAttachment();

		framebufferManager.AddColorAttachment(m_framebuffer, scColorAttach);
		framebufferManager.SetDepthStencilAttachment(m_framebuffer, scDepthAttach);
	}
}
