// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLFW

#include "Graphics/Context/GraphicsContext.h"

#include "Graphics/Context/GraphicsContextDescriptor.h"

class GLFWwindow;

namespace moe
{
	class GlfwGraphicsContext : public IGraphicsContext
	{
	public:

		typedef	GLFWwindow* GlfwWindowHandle;

		GlfwGraphicsContext() = default;
		~GlfwGraphicsContext() = default;

		virtual void	InitPreWindowCreation(const GraphicsContextDescriptor& contextDesc) override;

		virtual void	InitPostWindowCreation(const GraphicsContextDescriptor& contextDesc, GenericWindowHandle windowHandle) override;

	};

}


#endif // MOE_GLFW