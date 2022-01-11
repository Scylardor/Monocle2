// Monocle source files - Alexandre Baron

#include "RenderService.h"

#include "Graphics/RenderQueue/RenderQueue.h"

#include "GraphicsSurface/GraphicsSurface.h"

namespace moe
{
	RenderService::RenderService(Engine& ownerEngine) :
		Service(ownerEngine)
	{

	}


	void RenderService::Update()
	{
		for (Renderer& rdr : m_renderers)
		{
			if (!rdr.Ready())
				continue;

			RenderQueue drawCallQueue;

			uint8_t passIndex = 0;
			for (auto& renderPass : rdr)
			{
				renderPass->Update(drawCallQueue, passIndex);
				passIndex++;
				MOE_ASSERT(passIndex < RenderQueueKey::MAX_PASSES);
			}

			rdr.MutRHI()->SubmitCommandBuffer(drawCallQueue.GetCommandBuffer());
		}
	}
}

