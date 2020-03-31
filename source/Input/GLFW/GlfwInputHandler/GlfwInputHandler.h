#pragma once

#ifdef MOE_GLFW

#include "Input/InputHandler/InputHandler.h"

#include "Core/Preprocessor/moeDLLVisibility.h"


namespace moe
{
	class MOE_DLL_API GlfwInputHandler : public InputHandler
	{
	public:
		GlfwInputHandler() = default;

		virtual bool	PumpEvents() override;

	};
}


#endif // MOE_GLFW