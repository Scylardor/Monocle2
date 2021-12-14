// Monocle source files - Alexandre Baron


#include "Renderer.h"

#include "GameFramework/Service/RenderService/GraphicsSurface/GraphicsSurface.h"

namespace moe
{

	void Renderer::MakeGeometryPass()
	{

	}


	void Renderer::AttachSurface(IGraphicsSurface& surface)
	{
		m_attachedSurface = &surface;
		surface.OnSurfaceLostEvent().Add<Renderer, &Renderer::OnAttachedSurfaceLost>(this);
	}


}
