#include "TestVkApplication.h"


namespace moe
{



	TestVkApplication::TestVkApplication(const moe::VulkanGlfwAppDescriptor& appDesc) :
		VulkanGlfwApplication(appDesc)
	{

	}

	void TestVkApplication::Run()
	{
		while (WindowIsOpened())
		{
			PollInputEvents();
			m_renderer.RenderFrame();
		}

		m_renderer.Shutdown();

	}

}