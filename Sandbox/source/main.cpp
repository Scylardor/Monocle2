// Monocle Game Engine source files - Alexandre Baron

#include "GameFramework/Engine/Engine.h"

#include "BasicQuad.h"
#include "BlinnPhong.h"
#include "CubeWorld.h"
#include "ImGUI.h"
#include "MutliViewportCubeWorld.h"
#include "Phong.h"


int main(int argc, char **argv)
{
	moe::Engine monocleEngine;

	monocleEngine.AddSimulation<moe::ImGUI>(argc, argv);

	monocleEngine.Run();


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
