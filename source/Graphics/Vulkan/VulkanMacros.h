#ifdef MOE_VULKAN

#pragma once

#include "Core/Preprocessor/moeAssert.h"

// Activating the dynamic dispatcher in order to easily use Vulkan EXTension functions.
// In order to avoid complex compile errors and weird crashes, it's MANDATORY that the vulkan.hpp include always goes after the define !
// Never include vulkan.hpp from elsewhere than here.
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

#define MOE_VK_CHECK(op)						\
	do											\
	{											\
		vk::Result err = op;					\
		if (err != vk::Result::eSuccess)		\
		{										\
			MOE_ERROR(moe::ChanGraphics,		\
				"Unexpected Vulkan Error: %s",	\
				vk::to_string(err));			\
			MOE_DEBUG_ASSERT(false);			\
		}										\
	} while (0)


#define MOE_VK_DEVICE_GETTER() \
	const MyVkDevice&	Device() const \
	{ \
		MOE_ASSERT(m_device); \
		return *m_device; \
	} \
	MyVkDevice& Device() \
	{ \
		MOE_ASSERT(m_device); \
		return *m_device; \
	}




#endif // MOE_VULKAN
