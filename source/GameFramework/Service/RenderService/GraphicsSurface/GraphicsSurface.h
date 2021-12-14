// Monocle Game Engine source files - Alexandre Baron

#pragma once
#include "Core/Delegates/Event.h"


namespace moe
{

	class IGraphicsSurface
	{
	public:

		using SurfaceResizedEvent = Event<void(int, int)>;
		using SurfaceLostEvent = Event<void()>;

		virtual ~IGraphicsSurface()
		{
			m_onSurfaceLost.Broadcast();
		}

		virtual void					SwapBuffers() = 0;

		virtual std::pair<int, int>		GetDimensions() = 0;

		SurfaceResizedEvent&	OnSurfaceResizedEvent()
		{
			return m_onSurfaceResized;
		}

		SurfaceLostEvent&	OnSurfaceLostEvent()
		{
			return m_onSurfaceLost;
		}

	protected:

		void	OnSurfaceResized(int newWidth, int newHeight)
		{
			m_onSurfaceResized.Broadcast(newWidth, newHeight);
		}

		void	OnSurfaceLost()
		{
			m_onSurfaceLost.Broadcast();
		}


	private:

		SurfaceResizedEvent	m_onSurfaceResized{};
		SurfaceLostEvent	m_onSurfaceLost{};

	};
}
