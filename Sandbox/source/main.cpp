// Monocle Game Engine source files - Alexandre Baron

#include "Graphics/Context/GraphicsContextDescriptor.h"
#include "Window/WindowDescriptor.h"
#include "Application/GlfwApplication/GlfwApplication.h"

int main()
{
	moe::GraphicsContextDescriptor contextDesc;
	moe::WindowDescriptor windowDesc(1024_width, 728_height, "Monocle Sandbox");
	moe::GlfwApplication app(contextDesc, windowDesc);

	return 0;

}