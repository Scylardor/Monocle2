#pragma once

#include "Core/Containers/AssetRegistry/ObjectRegistry.h"
#include "Graphics/Vulkan/Shader/VulkanShaderProgram.h"


namespace moe
{
	class IShaderFactory
	{

	};


	class VulkanShaderFactory : public IShaderFactory
	{

	public:
		VulkanShaderFactory(MyVkDevice& device);


		void	BuildShaderProgram();


	private:

		MyVkDevice&	m_device;



		ObjectRegistry<VulkanShader>		m_shaders;
		ObjectRegistry<VulkanShaderProgram>	m_programs;


	};


}
