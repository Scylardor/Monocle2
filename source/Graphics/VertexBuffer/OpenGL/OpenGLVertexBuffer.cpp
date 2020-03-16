// Monocle Game Engine source files - Alexandre Baron

#include "OpenGLVertexBuffer.h"

#include <glad/glad.h>

moe::OpenGLVertexBuffer::OpenGLVertexBuffer(const BufferStorageDescription& storageDesc) :
	VertexBuffer(storageDesc)
{
	Create();

}


void moe::OpenGLVertexBuffer::Create()
{

}
