#include "OpenGLGraphicsDevice.h"

#include "Graphics/DeviceBuffer/OpenGL/OpenGLDeviceBuffer.h"

#include <glad/glad.h>


void moe::OpenGLGraphicsDevice::UpdateBuffer(DeviceBuffer& buffer, const byte_t* data, uint32_t sizeInBytes, uint32_t bufferOffsetInBytes)
{
	// The OpenGL graphics device is only compatible with OpenGL device buffers
	OpenGLDeviceBuffer& glBuffer = static_cast<OpenGLDeviceBuffer&>(buffer);

	glNamedBufferSubData(glBuffer.Name(), bufferOffsetInBytes, sizeInBytes, data);
}
