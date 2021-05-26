
#ifdef MOE_VULKAN
#include "VulkanShaderModule.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"


namespace moe
{
	VulkanShaderModule::~VulkanShaderModule()
	{
		if (m_device && m_pipelineStageInfo.module)
			(*m_device)->destroyShaderModule(m_pipelineStageInfo.module);
	}

}


#endif // MOE_VULKAN
