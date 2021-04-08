// Monocle Game Engine source files - Alexandre Baron

#if defined(MOE_GLFW) && defined(MOE_VULKAN)

#include "VulkanGlfwApplication.h"

#include "Application/GlfwApplication/Vulkan/VulkanGlfwAppDescriptor.h"

#include "Graphics/Vulkan/Instance/VulkanInstance.h"

#include "Graphics/Vulkan/VulkanMacros.h"

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
		return;
	}

	uint32_t extensionCount;
	const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
	if (CheckGLFWError() != GLFW_NO_ERROR)
	{
		m_initialized = false;
		return;
	}

	VulkanInstance::CreationParams instanceParms{ { extensionCount, extensions }, "Hello Vulkan", "Monocle" };
	m_initialized = m_renderer.Initialize(std::move(instanceParms), *this);

	if (m_initialized == false)
	{
		MOE_ERROR(moe::ChanWindowing, "Monocle GLFW Vulkan Interface could not initialize !");
		return;
	}


}


moe::VulkanGlfwApplication::~VulkanGlfwApplication()
{

}


void moe::VulkanGlfwApplication::OnWindowResized(int newWidth, int newHeight)
{
	for (auto& surfaceResizedCb : m_framebufferResizedCallbacks)
	{
		surfaceResizedCb(Width_t(newWidth), Height_t(newHeight));
	}
}


vk::SurfaceKHR moe::VulkanGlfwApplication::CreateSurface(VkInstance instance)
{
	MOE_ASSERT(m_initialized);
	VkSurfaceKHR newSurface;
	VkResult ok = glfwCreateWindowSurface(instance, GetGlfwWindow(), nullptr, &newSurface);
	MOE_ASSERT(ok == VK_SUCCESS);
	return newSurface;
}


moe::IVulkanSurfaceProvider::SurfaceDimensions moe::VulkanGlfwApplication::GetSurfaceDimensions()
{
	MOE_ASSERT(m_initialized);

	SurfaceDimensions dimensions{};
	glfwGetFramebufferSize(GetGlfwWindow(), (int*)&dimensions.Width, (int*)&dimensions.Height);
	return dimensions;
}


void moe::VulkanGlfwApplication::RegisterSurfaceResizeCallback(SurfaceResizeCallback&& cb)
{
	m_framebufferResizedCallbacks.emplace_back(std::move(cb));
}


#endif // defined(MOE_GLFW) && defined(MOE_VULKAN)
