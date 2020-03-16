#pragma once

#include "Application/GlfwApplication/GlfwApplication.h"

namespace moe
{


class TestApplication : public moe::GlfwApplication
{
public:

	TestApplication(const moe::GraphicsContextDescriptor& contextDesc, const moe::WindowDescriptor& windowDesc);
	~TestApplication() = default;

	void	Run();


};

}