// Monocle source files - Alexandre Baron

#include "GeometryRenderPass.h"

#include "GameFramework/Service/RenderService/Renderer/Renderer.h"

#include "Graphics/RenderQueue/RenderQueue.h"
#include "GameFramework/Service/RenderService/RenderScene/RenderScene.h"


namespace moe
{
	GeometryRenderPass::GeometryRenderPass(Renderer& owner) :
		m_ownerRenderer(&owner)
	{

	}


	GeometryRenderPass::~GeometryRenderPass()
	{
	}


	RenderQueueKey GeometryRenderPass::Update(RenderQueue& drawQueue, RenderQueueKey key, int /*subpassIdx*/)
	{
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

				renderedScene.UpdateObjectViewMatrices(object.GetTransformID(), view.GetViewMatrix(), view.GetViewProjectionMatrix());
				drawQueue.EmplaceDrawCall<CmdDrawMesh>(key, object.GetMeshHandle(), object.GetDynamicSetsHandle());
			}

			viewportIdx++;
		}

		return key;
	}




}
