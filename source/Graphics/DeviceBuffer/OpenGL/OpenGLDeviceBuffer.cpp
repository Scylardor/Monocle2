// Monocle Game Engine source files - Alexandre Baron

#include "OpenGLDeviceBuffer.h"

#include <glad/glad.h>

namespace moe
{
	OpenGLDeviceBuffer::OpenGLDeviceBuffer(const BufferStorageDescription& desc) :
		DeviceBuffer(desc.m_bufferUsage)
	{
		glCreateBuffers(1, &m_bufferName);
		MOE_DEBUG_ASSERT(m_bufferName != 0);

		UpdateData(desc);
	}


	OpenGLDeviceBuffer::~OpenGLDeviceBuffer()
	{
		glDeleteBuffers(1, &m_bufferName);
	}


	void OpenGLDeviceBuffer::UpdateData(const BufferStorageDescription& desc)
	{
		MOE_DEBUG_ASSERT(desc.m_data != nullptr);


	}


}