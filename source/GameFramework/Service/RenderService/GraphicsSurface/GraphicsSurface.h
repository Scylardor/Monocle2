// Monocle Game Engine source files - Alexandre Baron

#pragma once
#include "Core/Delegates/Event.h"


namespace moe
{

	class IGraphicsSurface
	{
	public:

		using SurfaceResizedEvent = Event<void(int, int)>;

		virtual ~IGraphicsSurface() = default;

		virtual void					SwapBuffers() = 0;

		virtual std::pair<int, int>		GetDimensions() = 0;

		SurfaceResizedEvent&	OnSurfaceResizedEvent()
		{
			return m_onSurfaceResized;
		}


	protected:

		void	OnSurfaceResized(int newWidth, int newHeight)
		{
			m_onSurfaceResized.Broadcast(newWidth, newHeight);
		}


	private:

		SurfaceResizedEvent	m_onSurfaceResized{};

	};
}
