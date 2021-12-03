// Monocle source files - Alexandre Baron

#pragma once
#include "RenderPass.h"
#include "Core/Delegates/EventDelegateID.h"

namespace moe
{
	class Renderer;

	class GeometryRenderPass : public IRenderPass
	{
	public:
		GeometryRenderPass(Renderer& owner);

		~GeometryRenderPass() override;


	private:

		void	OnGraphicsSurfaceResized(int newWidth, int newHeight);


		Renderer* m_ownerRenderer{ nullptr };

		moe::EventDelegateID	m_surfaceResizeDelID{};

	};



}
