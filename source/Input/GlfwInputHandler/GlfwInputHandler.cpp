
#ifdef MOE_GLFW

#include "GlfwInputHandler.h"

#include <GLFW/glfw3.h>

bool moe::GlfwInputHandler::PumpEvents()
{
	glfwPollEvents();

	// TODO : dispatch input commands

	return false;
}



#endif // MOE_GLFW
