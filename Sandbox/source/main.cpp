// Monocle Game Engine source files - Alexandre Baron

#ifdef MOE_VULKAN
#include "TestVkApplication.h"
#include "Application/GlfwApplication/Vulkan/VulkanGlfwAppDescriptor.h"
#endif

#include "Application/GlfwApplication/OpenGL/OpenGLGlfwApplication.h"
#include "TestApplication.h"

#include "Core/Misc/Literals.h"


//#include "btBulletCollisionCommon.h"


int main()
{
	//moe::VulkanGlfwAppDescriptor vkAppDesc(1024_width, 728_height, "Monocle Sandbox");

	//moe::BasicVkApp vkApp(vkAppDesc);

	//if (vkApp.IsInitialized())
	//	vkApp.Run();

	moe::OpenGLGlfwAppDescriptor appDesc(1024_width, 728_height, "Monocle Sandbox");

	moe::TestApplication app(appDesc);

	if (app.IsInitialized())
	{
		app.Run();
	}

	return 0;
}
