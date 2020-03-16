// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/DeviceBuffer/DeviceBuffer.h"

#include "Graphics/DeviceBuffer/BufferDescription.h"

namespace moe
{
	/* Base class for Vertex Buffers (API agnostic) */
	class VertexBuffer : public DeviceBuffer
	{
	public:
		VertexBuffer(const BufferStorageDescription& bufferDesc);

		virtual ~VertexBuffer() = default;

	};

}