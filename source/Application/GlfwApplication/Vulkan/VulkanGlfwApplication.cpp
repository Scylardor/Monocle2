// Monocle Game Engine source files - Alexandre Baron

#if defined(MOE_GLFW) && defined(MOE_VULKAN)

#include "VulkanGlfwApplication.h"

#include "Application/GlfwApplication/Vulkan/VulkanGlfwAppDescriptor.h"

#include "Graphics/RHI/Vulkan/VulkanRHI.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>



moe::VulkanGlfwApplication::VulkanGlfwApplication(const moe::VulkanGlfwAppDescriptor& appDesc) :
	m_description(appDesc)
{
	if (false == m_initialized)
		return;

	if (glfwVulkanSupported() == false)
	{
		MOE_ERROR(moe::ChanWindowing, "Vulkan is not supported by GLFW !");
		m_initialized = false;
		return;
	}

	// Because GLFW was originally designed to create an OpenGL context, we need to tell it to not create an OpenGL context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	CreateGlfwWindow(m_description);
	m_initialized = (GetGlfwWindow() != nullptr);

	if (false == m_initialized)
	{
		MOE_ERROR(moe::ChanWindowing, "Failed to create GLFW window.");
	}

	RequiredExtensionList extensionList;
	extensionList.ExtensionList = glfwGetRequiredInstanceExtensions(&extensionList.ExtensionNumber);

	if (CheckGLFWError() != GLFW_NO_ERROR)
	{
		m_initialized = false;
	}

}


moe::VulkanGlfwApplication::~VulkanGlfwApplication()
{

}


#endif // defined(MOE_GLFW) && defined(MOE_VULKAN)
