#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"
#include "Core/Resource/Resource.h"

namespace moe
{
	class MyVkDevice;

	class VulkanShaderModule : public ShaderResource
	{
	public:

		VulkanShaderModule(MyVkDevice& device, vk::ShaderModule module, vk::ShaderStageFlagBits stageFlags, std::string_view entryPoint = "main") :
			m_device(&device),
			m_entryPoint(entryPoint),
			m_pipelineStageInfo(vk::PipelineShaderStageCreateFlags{}, stageFlags, module, m_entryPoint.data())
		{}

		~VulkanShaderModule();


		vk::ShaderModule	Module() const
		{
			return m_pipelineStageInfo.module;
		}


		const vk::PipelineShaderStageCreateInfo&	GetCreateInfo() const
		{
			return m_pipelineStageInfo;
		}

	private:
		MyVkDevice*							m_device{nullptr};
		std::string							m_entryPoint{ "main" };
		vk::PipelineShaderStageCreateInfo	m_pipelineStageInfo{};
	};
}


#endif // MOE_VULKAN
