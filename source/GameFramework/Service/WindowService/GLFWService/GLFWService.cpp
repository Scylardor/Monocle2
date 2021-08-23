// Monocle Game Engine source files - Alexandre Baron

#ifdef MOE_GLFW

#include "GLFWService.h"

#include "Core/Exception/RuntimeException.h"
namespace moe
{

	GLFWService::GLFWService(Engine& engine) :
		WindowService(engine)
	{
		auto init = glfwInit();

		if (init == GLFW_FALSE)
		{
			const char* why;
			glfwGetError(&why);
			std::string error;
			moe::StringFormat(error, "Could not initialize GLFW: error '%s'", why);
			throw RuntimeException(error);
		}

		glfwSetErrorCallback(&ErrorCallback);
	}


	GLFWService::~GLFWService()
	{
		glfwTerminate();
	}


	void GLFWService::ErrorCallback(int code, const char* description)
	{
		MOE_ERROR(moe::ChanWindowing, "GLFW error code %i: '%s'", code, description);
		MOE_DEBUG_ASSERT(false);
	}
}


#endif