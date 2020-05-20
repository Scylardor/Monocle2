// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/Resources/ResourceLayout/ResourceLayoutHandle.h"
#include "Graphics/Shader/Handle/ShaderHandle.h"

#include "Graphics/Material/MaterialBindings.h"

#include "Core/Containers/Vector/Vector.h"


namespace moe
{

	/**
	 * \brief A material interface contains the resource layout, shader program associated with this material,
	 * and also lists of bindings matching uniform block bindings and texture bindings.
	 * They cannot be used as is : you must first create a material instance from them.
	 */
	class MaterialInterface
	{

	public:

		MaterialInterface() = default;

		MaterialInterface(ResourceLayoutHandle resourceLayout) :
			m_layoutHandle(resourceLayout)
		{}

		void	AddBlockBinding(MaterialBlockBinding blockBinding)
		{
			m_materialBlockBindings.PushBack(blockBinding);
		}

		void	AddTextureBinding(MaterialTextureBinding textureBinding)
		{
			m_textureUnitBindings.PushBack(textureBinding);
		}

		void	SetResourceLayout(ResourceLayoutHandle layoutHandle)
		{
			m_layoutHandle = layoutHandle;
		}

		void	SetShaderProgram(ShaderProgramHandle programHandle)
		{
			m_programHandle = programHandle;
		}


		[[nodiscard]] const Vector<MaterialBlockBinding>&	GetBlockBindings() const { return m_materialBlockBindings; }

		[[nodiscard]] ResourceLayoutHandle	GetResourceLayout() const { return m_layoutHandle; }

		[[nodiscard]]  ShaderProgramHandle	GetShaderProgram() const { return m_programHandle; }

	private:

		Vector<MaterialBlockBinding>	m_materialBlockBindings;
		Vector<MaterialTextureBinding>	m_textureUnitBindings;

		ShaderProgramHandle		m_programHandle;
		ResourceLayoutHandle	m_layoutHandle;

	};


}