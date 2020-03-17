#pragma once

#ifdef MOE_GLFW

#include "Input/InputHandler/InputHandler.h"


namespace moe
{
	class GlfwInputHandler : public InputHandler
	{
	public:
		GlfwInputHandler() = default;

		virtual bool	PumpEvents() override;

	};
}


#endif // MOE_GLFW