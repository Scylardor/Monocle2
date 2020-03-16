// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/VertexBuffer/VertexBuffer.h"
#include "Graphics/DeviceBuffer/BufferDescription.h"

namespace moe
{
	/* Implementation of an OpenGL Vertex Buffer */
	class OpenGLVertexBuffer final : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(const BufferStorageDescription& storageDesc);

		void	Create();

	private:
		unsigned int m_name{ 0 }; // OpenGL ID of this buffer
	};
}