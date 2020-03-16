// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/DeviceBuffer/DeviceBuffer.h"

namespace moe
{
	class OpenGLDeviceBuffer : public DeviceBuffer
	{
	public:
		OpenGLDeviceBuffer(const BufferStorageDescription& desc);
		~OpenGLDeviceBuffer();

		virtual void	UpdateData(const BufferStorageDescription& desc) override;

		unsigned int	Name() const { return m_bufferName; }

	private:

		unsigned int m_bufferName{ 0 }; // OpenGL ID of the buffer
	};


}