
#include "TestApplication.h"


TestApplication::TestApplication(const moe::GraphicsContextDescriptor& contextDesc, const moe::WindowDescriptor& windowDesc) :
	moe::GlfwApplication(contextDesc, windowDesc)
{
}


void TestApplication::Run()
{
	while (!m_window->ShouldWindowClose())
	{
		m_inputHandler->PumpEvents();
	}

}
