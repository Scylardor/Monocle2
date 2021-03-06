// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

#include "Graphics/VertexLayout/VertexLayout.h"

#include <glad/glad.h>

namespace moe
{


	/**
	 * \brief A thin wrapper over an OpenGL Vertex Array Object (VAO) in RAII-fashion that automatically deletes itself when going out of scope.
	 * It is handy to avoid forgetting to delete them.
	 */
	class OpenGLVertexLayout : public VertexLayout
	{
		static const GLuint ms_nullVao = 0;

	public:

		OpenGLVertexLayout(const VertexLayoutDescriptor& desc, GLuint vaoID, uint32_t totalSizeBytes) :
		VertexLayout(desc),
		m_vaoId(vaoID),
		m_strideInBytes(totalSizeBytes)
		{}

		OpenGLVertexLayout(InstancedVertexLayoutDescriptor&& desc, GLuint vaoID) :
			VertexLayout(std::move(desc)),
			m_vaoId(vaoID)
		{}


		~OpenGLVertexLayout() = default;


		operator GLuint() const { return m_vaoId; }


		GLuint	VAO() const { return m_vaoId; }


		void	SetStrideBytes(uint32_t size)
		{
			m_strideInBytes = size;
		}

		uint32_t	GetStrideBytes() const { return m_strideInBytes; }


	private:
		GLuint	m_vaoId{ms_nullVao};

		// Only really useful for Interleaved layouts, but it's a reasonable overhead even if unused.
		uint32_t	m_strideInBytes = 0;


	};
}

#endif