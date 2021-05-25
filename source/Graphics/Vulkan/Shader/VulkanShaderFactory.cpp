#ifdef MOE_VULKAN

#include "VulkanShaderFactory.h"

namespace moe
{

	std::unique_ptr<ShaderResource> VulkanShaderFactory::LoadShaderBytecode(std::string_view /*filename*/, vk::ShaderStageFlagBits /*shaderStage*/)
	{
		return {};
	}

	RegistryID VulkanShaderFactory::IncrementReference(RegistryID id)
	{
		return id;
	}


	bool VulkanShaderFactory::DecrementReference(RegistryID id)
	{
		id;
		return true;
	}
}


#endif // MOE_VULKAN
