#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"
#include "Core/Resource/ResourceFactory.h"
#include "Core/Resource/Resource.h"
#include "VulkanShaderProgram.h"

namespace moe
{
	class MyVkDevice;

	class IShaderFactory : public IResourceFactory
	{
	public:

		[[nodiscard]] virtual std::unique_ptr<ShaderResource>	LoadShaderBytecode(std::string_view filename, vk::ShaderStageFlagBits shaderStage) = 0;


	};


	class VulkanShaderResource : public ShaderResource
	{

		
	};


	class VulkanShaderProgramResource : public ShaderProgramResource
	{


	};


	class VulkanShaderFactory : public IShaderFactory
	{


	public:

		VulkanShaderFactory() = default;

		VulkanShaderFactory(MyVkDevice & device) :
			m_device(&device)
		{}

		~VulkanShaderFactory() override = default;


		[[nodiscard]] std::unique_ptr<ShaderResource>	LoadShaderBytecode(std::string_view filename, vk::ShaderStageFlagBits shaderStage) override;

		RegistryID	IncrementReference(RegistryID id) override;
		bool		DecrementReference(RegistryID id) override;

	protected:

	private:

		MyVkDevice* m_device{ nullptr };

		ObjectRegistry<vk::ShaderModule>	m_shaders;
		ObjectRegistry<VulkanShaderProgram>	m_programs;

	};
}


#endif // MOE_VULKAN
