#pragma once
#ifdef MOE_VULKAN
#include "Application/GlfwApplication/Vulkan/VulkanGlfwApplication.h"

#include "Application/GlfwApplication/Vulkan/VulkanGlfwAppDescriptor.h"

namespace moe
{


	class TestVkApplication : public moe::VulkanGlfwApplication
	{
	public:

		TestVkApplication(const moe::VulkanGlfwAppDescriptor& appDesc);
		~TestVkApplication() = default;

		void	Run();

		virtual void	Update() {}


	};


	class BasicVkApp : public TestVkApplication
	{
	public:
		BasicVkApp(const moe::VulkanGlfwAppDescriptor& appDesc);


	};
}
#endif