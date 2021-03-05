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

#endif // MOE_VULKAN
