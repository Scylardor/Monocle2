// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include <string>

#include "Core/Containers/HashMap/HashMap.h"

#include "Graphics/Resources/ResourceLayout/ResourceKind.h"
#include "Graphics/Shader/Handle/ShaderHandle.h"

#include "MaterialInterface.h"
#include "MaterialInstance.h"
#include "MaterialDescriptor.h"


#include "Monocle_Graphics_Export.h"


namespace moe
{
	class IGraphicsDevice;

	struct MaterialBindingInfo
	{
		uint16_t		m_bindingPoint{ 0 };
		ResourceKind	m_resourceKind{ ResourceKind::None };
	};


	class MaterialLibrary
	{

	public:

		explicit MaterialLibrary(IGraphicsDevice& device) :
			m_device(device)
		{}


		using UniformBufferSizer = std::size_t(*)();

		void	AddBindingMapping(std::string name, MaterialBindingInfo bindingInfo)
		{
			m_bindingMappings.Insert({ std::move(name), std::move(bindingInfo) });
		}

		MaterialInterface GenerateNewMaterial(ShaderProgramHandle progHandle, const MaterialDescriptor& matDesc);


		void	AddUniformBufferSizer(MaterialBlockBinding blockBinding, UniformBufferSizer sizer)
		{
			m_bufferSizers.Insert({ blockBinding, sizer });
		}


		[[nodiscard]] std::size_t GetUniformBufferSize(const std::string& uboSemantic) const;


		[[nodiscard]] std::size_t GetUniformBufferSize(uint16_t uboBindingPoint) const;

		Monocle_Graphics_API [[nodiscard]] MaterialInterface	CreateMaterialInterface(ShaderProgramHandle progHandle, const MaterialDescriptor& matDesc);

		Monocle_Graphics_API [[nodiscard]] MaterialInstance		CreateMaterialInstance(const MaterialInterface& materialIntf);


	private:
		IGraphicsDevice&							m_device;
		HashMap<std::string, MaterialBindingInfo>	m_bindingMappings;
		HashMap<uint16_t, UniformBufferSizer>		m_bufferSizers;

	};


}
