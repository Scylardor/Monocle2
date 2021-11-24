// Monocle source files - Alexandre Baron

#pragma once
#include "RenderPass.h"

namespace moe
{
	class Renderer;

	class GeometryRenderPass : public IRenderPass
	{
	public:
		GeometryRenderPass(Renderer& owner) :
			m_ownerRenderer(&owner)
		{}


	private:
		Renderer* m_ownerRenderer{ nullptr };
	};



}
