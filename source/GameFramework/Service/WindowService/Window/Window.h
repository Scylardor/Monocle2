// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "GameFramework/Service/RenderService/GraphicsSurface/GraphicsSurface.h"

#include <GameFramework/Service/InputService/InputSource.h>

namespace moe
{

	class IWindow : public IGraphicsSurface, public InputSource

	{
	public:

		using WindowClosedEvent = Event<void()>;

		virtual bool	ShouldClose() = 0;


	};
}
