#ifdef MOE_VULKAN

#include "VulkanRHI.h"


moe::VulkanRHI::~VulkanRHI()
{
	// Destroy the Vulkan Instance
}


bool moe::VulkanRHI::Initialize(VulkanInstance::CreationParams&& instanceParams)
{
	return (m_instance.Initialize(std::move(instanceParams)));
}


#endif // MOE_VULKAN
