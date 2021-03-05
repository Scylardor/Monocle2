// Monocle Game Engine source files - Alexandre Baron

#include "Core/Misc/Literals.h"

#if defined(MOE_GLFW) && defined(MOE_VULKAN)

#include "VulkanGlfwAppDescriptor.h"

#include <GLFW/glfw3.h> // GLFW_OPENGL_CORE_PROFILE

namespace moe
{
	VulkanGlfwAppDescriptor::VulkanGlfwAppDescriptor(Width_t w, Height_t h, const char* windowTitle, const char* iconPath, bool resizableWin, uint32_t numSamples) :
		AppDescriptor(w, h, windowTitle, iconPath, resizableWin, numSamples)
	{}
}

#endif // defined(MOE_GLFW) && defined(MOE_VULKAN)