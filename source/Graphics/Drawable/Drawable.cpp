#include "Drawable.h"

#include "Graphics/Vulkan/VulkanMacros.h"

#include "Graphics/Vulkan/Pipeline/VulkanPipeline.h"


namespace moe
{

	void Drawable::BindTransform(VulkanPipeline const& pipeline, vk::CommandBuffer command) const
	{
		auto& mvp = MVP();
		command.pushConstants(pipeline.PipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(Mat4), &mvp);
	}

}
