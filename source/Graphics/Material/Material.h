// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Containers/Vector/Vector.h"

#include "Graphics/Shader/Handle/ShaderHandle.h"

#include "MaterialObjectBlock.h"
#include "MaterialBlock.h"

#include "Graphics/Resources/ResourceLayout/ResourceLayoutHandle.h"

#include "Graphics/Resources/ResourceSet/ResourceSetHandle.h"

#include "Graphics/OpenGL/Std140.h"

namespace moe
{



	struct ObjectMatrices
	{
		ObjectMatrices() = default;

		Mat4			m_model;
		Mat4			m_modelView;
		Mat4			m_modelViewProjection;
		Std140Mat<3>	m_normalMatrix;
	};


	class IGraphicsRenderer;
	class RenderWorld;
	class AGraphicObject;


	struct ResourceDataHandles
	{
		ResourceLayoutHandle	m_layoutHandle{0};
		ResourceSetHandle		m_setHandle{0};
	};


	class Material
	{
	public:

		Material(IGraphicsRenderer& renderer, ShaderProgramHandle shaderHandle) :
			m_renderer(renderer), m_programHandle(shaderHandle)
		{}

		void	AddPerMaterialResourceSet(ResourceSetHandle setHandle)
		{
			m_perMaterialResources.PushBack(setHandle);
		}

		void	AddPerObjectResourceSet(ResourceSetHandle setHandle)
		{
			m_perObjectResources.PushBack(setHandle);
		}

		[[nodiscard]] ShaderProgramHandle	GetShaderProgramHandle() const { return m_programHandle; }

		[[nodiscard]] const Vector<ResourceSetHandle>&	GetPerMaterialResourceSets() const { return m_perMaterialResources; }
		[[nodiscard]] const Vector<ResourceSetHandle>&	GetPerObjectResourceSets() const { return m_perObjectResources; }


		void		SetFrameUniformBlockCounter(uint32_t count) { m_frameUniformBindingCounter = count; }

		void		ResetFrameUniformBlockCounter() { m_frameUniformBindingCounter = 0; }

		[[nodiscard]] uint32_t	GetFrameUniformBlockCounter() const	{ return m_frameUniformBindingCounter; }


		static void	UpdateObjectMatrices(AGraphicObject& object, DeviceBufferHandle ubHandle);

	private:

		IGraphicsRenderer&		m_renderer;

		ShaderProgramHandle		m_programHandle{ 0 };

		Vector<ResourceSetHandle>	m_perMaterialResources;
		Vector<ResourceSetHandle>	m_perObjectResources;

		uint32_t	m_frameUniformBindingCounter = 0;
	};

}
