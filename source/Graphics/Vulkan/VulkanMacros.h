#ifdef MOE_VULKAN

#pragma once

#include "Core/Preprocessor/moeAssert.h"
#include <vulkan/vulkan.hpp>

#define MOE_VK_CHECK(op)						\
	do											\
	{											\
		vk::Result err = op;					\
		if (err)								\
		{										\
			MOE_ERROR(moe::ChanGraphics,		\
				"Unexpected Vulkan Error: %s",	\
				vk::to_string(err));			\
			MOE_DEBUG_ASSERT(false);			\
		}										\
	} while (0)

namespace moe
{
#ifdef MOE_DEBUG
	static const bool	S_enableValidationLayers = true;
#else
	static const bool	S_enableValidationLayers = false;
#endif

}


#endif // MOE_VULKAN
