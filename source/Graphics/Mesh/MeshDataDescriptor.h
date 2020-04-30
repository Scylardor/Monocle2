// Monocle Game Engine source files - Alexandre Baron

#pragma once


namespace moe
{
	/**
	 * \brief This is mostly a helper structure to submit static mesh data to the GPU.
	 */
	struct MeshDataDescriptor
	{
		bool	IsNull() const { return (m_dataBuffer == nullptr || m_bufferNumElems == 0); }

		const void*	m_dataBuffer = nullptr; // The pointer to raw data.
		size_t		m_bufferSizeBytes = 0; // The total size of the buffer, in bytes.
		size_t		m_bufferNumElems = 0; // The number of elements in the buffer (e.g. number of vertices of a vertex buffer).
	};
}