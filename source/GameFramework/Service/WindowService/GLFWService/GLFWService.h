// Monocle Game Engine source files - Alexandre Baron

#pragma once
#ifdef MOE_GLFW

#include <GLFW/glfw3.h>

#include "GameFramework/Service/WindowService/WindowService.h"

namespace moe
{
	class GLFWService : public WindowService
	{
	public:

		using ServiceLocatorType = WindowService;

		GLFWService(Engine& engine);

		~GLFWService() override;


	private:

		static void	ErrorCallback(int code, const char* description);


	};

}


#endif