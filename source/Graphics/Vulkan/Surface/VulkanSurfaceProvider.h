#pragma once
#ifdef MOE_VULKAN

#include <vulkan/vulkan.h>

#include "Core/Misc/Literals.h"

namespace moe
{
	class IVulkanSurfaceProvider
	{
	public:
		virtual ~IVulkanSurfaceProvider() = default;

		/* Creates a new surface from this provider. */
		virtual vk::SurfaceKHR	CreateSurface(VkInstance instance) = 0;


		struct SurfaceDimensions
		{
			Width_t		Width = 0_width;
			Height_t	Height = 0_height;
		};
		virtual SurfaceDimensions	GetSurfaceDimensions() = 0;

	};
}


#endif // MOE_VULKAN


