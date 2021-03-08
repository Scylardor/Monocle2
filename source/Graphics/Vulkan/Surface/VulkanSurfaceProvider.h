#pragma once
#ifdef MOE_VULKAN

#include <vulkan/vulkan.h>

#include <utility> // std::pair

#include "Core/Misc/Literals.h"

namespace moe
{
	class IVulkanSurfaceProvider
	{
	public:
		virtual ~IVulkanSurfaceProvider() = default;

		virtual VkSurfaceKHR	GetSurface(VkInstance instance) = 0;

		struct SurfaceDimensions
		{
			Width_t		Width = 0_width;
			Height_t	Height = 0_height;
		};
		virtual SurfaceDimensions	GetSurfaceDimensions() = 0;

	};
}


#endif // MOE_VULKAN


