
#ifdef MOE_VULKAN
#include "Drawable.h"

#include "Graphics/Vulkan/VulkanMacros.h"

#include "Graphics/Vulkan/Pipeline/VulkanPipeline.h"


namespace moe
{

	void Drawable::BindPerObjectResources(VulkanPipeline const& pipeline, vk::CommandBuffer command) const
	{
		command.pushConstants(pipeline.PipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(Mat4), &m_mvp);
	}

}
#endif