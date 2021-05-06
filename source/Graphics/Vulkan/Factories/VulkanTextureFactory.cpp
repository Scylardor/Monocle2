#include "VulkanTextureFactory.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"

namespace moe
{
	VulkanTextureFactory::VulkanTextureFactory(MyVkDevice& device) :
		m_device(device)
	{
	}
}
