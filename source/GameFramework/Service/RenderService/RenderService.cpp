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
			RenderQueue drawCallQueue;

			uint8_t passIndex = 0;
			for (auto& renderPass : rdr)
			{
				renderPass->Update(drawCallQueue, passIndex);
				passIndex++;
				MOE_ASSERT(passIndex <= GetMaxRenderPassIndex());
			}

		}
	}
}

