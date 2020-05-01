// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"

#include "Graphics/DeviceBuffer/VertexBufferHandle.h"
#include "Graphics/DeviceBuffer/IndexBufferHandle.h"

namespace moe
{


	/**
	 * \brief A graphics API-agnostic way of representing a 3D mesh.
	 * It will hold all the vertices information the graphics API needs to draw this mesh.
	 * Optionally, you can reference an index buffer to make this an indexed mesh.
	 * Otherwise, it is considered to contain only pure geometry.
	 */
	class Mesh
	{
	public:

		Mesh() = default;

		Mesh(VertexBufferHandle handle, size_t numVerts, IndexBufferHandle indexHandle = IndexBufferHandle::Null(), size_t numIndices = 0) :
			m_vertexHandle(handle),
			m_numVertices(numVerts),
			m_indexHandle(indexHandle),
			m_numIndices(numIndices)
		{}

		[[nodiscard]] VertexBufferHandle	GetVertexBufferHandle() const { return m_vertexHandle; }
		[[nodiscard]] IndexBufferHandle		GetIndexBufferHandle() const { return m_indexHandle; }

	private:

		VertexBufferHandle	m_vertexHandle{0};
		size_t				m_numVertices{ 0 };

		IndexBufferHandle	m_indexHandle{0};
		size_t				m_numIndices{0};
	};

}