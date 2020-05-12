// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Containers/Vector/Vector.h"

#include "Graphics/Shader/Handle/ShaderHandle.h"

#include "MaterialObjectBlock.h"
#include "MaterialBlock.h"
#include "MaterialFrameBlock.h"

#include <Core/Containers/Array/Array.h>

#include "Graphics/Resources/ResourceLayout/ResourceLayoutHandle.h"

#include "Math/Matrix.h"

#include "Graphics/Resources/ResourceSet/ResourceSetHandle.h"
#include "Graphics/Resources/ResourceLayout/ResourceLayoutDescriptor.h"
#include "Graphics/Texture/TextureDescription.h"


namespace moe
{

	template <size_t N, typename T = float, typename Enable = void>
	struct Std140Mat;

	template <size_t N, typename T>
	struct Std140Mat<N, T, typename std::enable_if_t<N == 2>>
	{
		Std140Mat(const Matrix<N, N, T>& mat) :
			m_vec1(mat[0]), m_vec2(mat[1])
		{}

		alignas(16) Vec<N, T> m_vec1;
		alignas(16) Vec<N, T> m_vec2;
	};

//#pragma warning( push )
//#pragma warning(disable : 4324)
	template <size_t N, typename T>
	struct Std140Mat<N, T, typename std::enable_if_t<N == 3>>
	{
		Std140Mat(const Matrix<N, N, T>& mat = Matrix<N, N, T>::Identity()) :
			m_vec1(mat[0]), m_vec2(mat[1]), m_vec3(mat[2])
		{}

		alignas(16) Vec<4, T> m_vec1;
		alignas(16) Vec<4, T> m_vec2;
		alignas(16) Vec<4, T> m_vec3;
	};
//#pragma warning( pop )

	template <size_t N, typename T>
	struct Std140Mat<N, T, typename std::enable_if_t<N == 4>>
	{
		Std140Mat(const Matrix<N, N, T>& mat) :
			m_vec1(mat[0]), m_vec2(mat[1]), m_vec3(mat[2]), m_vec4(mat[3])
		{}

		alignas(16) Vec<4, T> m_vec1;
		alignas(16) Vec<4, T> m_vec2;
		alignas(16) Vec<4, T> m_vec3;
		alignas(16) Vec<4, T> m_vec4;
	};


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
