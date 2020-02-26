// Monocle Game Engine source files - Alexandre Baron

#include "Graphics/Context/GraphicsContextDescriptor.h"
#include "Window/WindowDescriptor.h"

#include "TestApplication.h"

int main()
{
	moe::GraphicsContextDescriptor contextDesc;
	moe::WindowDescriptor windowDesc(1024_width, 728_height, "Monocle Sandbox");

	TestApplication app(contextDesc, windowDesc);

	if (app.IsInitialized())
	{
		app.Run();
	}

	return 0;

}