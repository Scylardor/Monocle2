// Monocle Game Engine source files - Alexandre Baron

#ifdef MOE_VULKAN
#include "TestVkApplication.h"
#include "Application/GlfwApplication/Vulkan/VulkanGlfwAppDescriptor.h"
#endif


#include "Application/GlfwApplication/OpenGL/OpenGLGlfwApplication.h"
#include "TestApplication.h"

#include "Core/Misc/Literals.h"
#include "GameFramework/Engine/Engine.h"
#include "GameFramework/Service/LogService/LogService.h"
#include "GameFramework/Service/TimeService/TimeService.h"
#include "GameFramework/Simulation/App3D/OpenGLApp3D.h"

#include "BasicQuad.h"



int main(int argc, char **argv)
{
	moe::Engine monocleEngine;

	monocleEngine.AddSimulation<moe::BasicQuad>(argc, argv);

	monocleEngine.Start();


	//std::optional<uint32_t> width = app3d.Get<moe::ConfigService>()->Get<uint32_t>("window:width");
	//std::optional<uint32_t> height = app3d.Get<moe::ConfigService>()->Get<uint32_t>("window:height");

	//moe::VulkanGlfwAppDescriptor vkAppDesc(1024_width, 728_height, "Monocle Sandbox");

	//moe::BasicVkApp vkApp(vkAppDesc);

	//if (vkApp.IsInitialized())
	//	vkApp.Start();

	//moe::OpenGLGlfwAppDescriptor appDesc(1024_width, 728_height, "Monocle Sandbox");
	//
	//moe::TestApplication app(appDesc);
	//
	//if (app.IsInitialized())
	//{
	//	app.Start();
	//}

	return 0;
}
