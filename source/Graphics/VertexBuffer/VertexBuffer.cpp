// Monocle Game Engine source files - Alexandre Baron

#include "VertexBuffer.h"

#include "Graphics/DeviceBuffer/BufferUsage.h"

namespace moe
{
	VertexBuffer::VertexBuffer(const BufferStorageDescription&) :
		DeviceBuffer(BufferUsage::VertexBuffer)
	{}


}