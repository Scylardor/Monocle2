// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "GlfwApplication.h"

#include <GLFW/glfw3.h>


moe::GlfwApplication::GlfwApplication(const WindowDescriptor& windowDesc)
{
	glfwInit();




}


moe::GlfwApplication::~GlfwApplication()
{
	glfwTerminate();
}
