#pragma once

#include "Application/GlfwApplication/OpenGL/OpenGLGlfwApplication.h"

namespace moe
{


class TestApplication final : public moe::OpenGLGlfwApplication
{
public:

	TestApplication(const moe::OpenGLGlfwAppDescriptor& appDesc);
	~TestApplication() = default;

	void	Run();


};

}