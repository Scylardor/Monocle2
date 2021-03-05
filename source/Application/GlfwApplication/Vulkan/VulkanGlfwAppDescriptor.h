// Monocle Game Engine source files - Alexandre Baron

#pragma once

#if defined(MOE_GLFW) && defined(MOE_VULKAN)

#include "Monocle_Application_Export.h"

#include "Application/AppDescriptor/AppDescriptor.h"

namespace moe
{
	/**
	 * \brief A descriptor for a GLFW application OpenGL context.
	 * Can be parameterized about major/minor OpenGL versions and needed OpenGL profile type (core/compat).
	 */
	struct VulkanGlfwAppDescriptor : public AppDescriptor
	{
	public:
		Monocle_Application_API VulkanGlfwAppDescriptor(Width_t w, Height_t h, const char* windowTitle = "Monocle Application", const char* iconPath = nullptr, bool resizableWin = false, uint32_t numSamples = 4);

		Monocle_Application_API ~VulkanGlfwAppDescriptor() = default;


	};
}

#endif // defined(MOE_GLFW) && defined(MOE_VULKAN)