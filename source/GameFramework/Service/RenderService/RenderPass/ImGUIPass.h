// Monocle source files - Alexandre Baron

#pragma once
#include "RenderPass.h"

namespace moe
{
	class Renderer;

	class ImGUIPass : public ISubpass
	{
	public:
		ImGUIPass(Renderer& owner);

		~ImGUIPass() override = default;

		RenderQueueKey	Update(RenderQueue& drawQueue, RenderQueueKey key, int subpassIdx) override;

	private:

		Renderer*				m_ownerRenderer{ nullptr };
	};



}
