
#ifdef MOE_VULKAN

#include "VulkanShaderFactory.h"
#include "Core/Misc/moeFile.h"
#include "Graphics/Vulkan/Device/VulkanDevice.h"
#include "VulkanShaderModule.h"

namespace moe
{

	std::unique_ptr<ShaderResource> VulkanShaderFactory::LoadShaderBytecode(std::string_view bytecodeFile, vk::ShaderStageFlagBits shaderStage)
	{
		MOE_ASSERT(m_device);

		// read the SPIRV bytecode and create the shader module.
		auto shaderCode = ReadBinaryFile(bytecodeFile);
		if (false == MOE_ASSERT(shaderCode.has_value()))
		{
			return nullptr;
		}

		vk::ShaderModuleCreateInfo createInfo{};
		createInfo.codeSize = shaderCode->size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode->data());

		vk::ShaderModule shaderModule = Device()->createShaderModule(createInfo);
		MOE_ASSERT(shaderModule);

		// TODO : no specialization or entry point specified for now...
		return std::make_unique<VulkanShaderModule>(*m_device, shaderModule, shaderStage, "main");
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
