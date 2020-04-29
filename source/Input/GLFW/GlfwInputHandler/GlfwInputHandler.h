#pragma once

#ifdef MOE_GLFW

#include "Input/InputHandler/InputHandler.h"

#include "Monocle_Input_Export.h"

#include "Monocle_Input_Export.h"

namespace moe
{
	class Monocle_Input_API GlfwInputHandler
	{
	public:
		GlfwInputHandler() = default;

		virtual bool	PumpEvents();

	};
}


#endif // MOE_GLFW