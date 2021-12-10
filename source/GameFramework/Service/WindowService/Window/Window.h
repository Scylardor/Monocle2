// Monocle Game Engine source files - Alexandre Baron

#pragma once
#include "GameFramework/Service/RenderService/GraphicsSurface/GraphicsSurface.h"


namespace moe
{

	class IWindow : public IGraphicsSurface
	{
	public:

		virtual ~IWindow() = default;

		enum class WindowState : char
		{
			Opened,
			Closed
		};

		virtual WindowState	PollEvents() = 0;
	};
}
