// Monocle source files - Alexandre Baron

#include "RenderService.h"

#include "Graphics/RenderQueue/RenderQueue.h"


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
				RenderQueueKey key = RenderQueue::ComputeRenderQueueKey(passIndex);


				key = renderPass->Update(drawCallQueue, key);


				passIndex++;
				MOE_ASSERT(passIndex < RenderQueueKey::MAX_PASSES);

			}

			rdr.MutRHI()->SubmitCommandBuffer(drawCallQueue.GetCommandBuffer());
		}
	}
}

