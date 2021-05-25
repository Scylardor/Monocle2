#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"
#include "Core/Resource/Resource.h"
#include "Graphics/Vulkan/Device/VulkanDevice.h"


namespace moe
{
	class VulkanShaderModule : public ShaderResource
	{
	public:

		VulkanShaderModule(MyVkDevice& device, vk::ShaderModule module, vk::ShaderStageFlagBits stageFlags, std::string_view entryPoint = "main") :
			m_device(&device),
			m_entryPoint(entryPoint),
			m_pipelineStageInfo(vk::PipelineShaderStageCreateFlags{}, stageFlags, module, m_entryPoint.data())
		{}

		~VulkanShaderModule()
		{
			if (m_device && m_pipelineStageInfo.module)
				(*m_device)->destroyShaderModule(m_pipelineStageInfo.module);
		}

	private:
		MyVkDevice*							m_device{nullptr};
		std::string							m_entryPoint{ "main" };
		vk::PipelineShaderStageCreateInfo	m_pipelineStageInfo{};
	};
}


#endif // MOE_VULKAN
