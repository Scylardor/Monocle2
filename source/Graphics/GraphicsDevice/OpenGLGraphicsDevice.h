// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/GraphicsDevice/GraphicsDevice.h"

namespace moe
{
	class OpenGLGraphicsDevice : public GraphicsDevice
	{
	public:
		OpenGLGraphicsDevice();

		virtual void	UpdateBuffer(class DeviceBuffer& buffer, const byte_t* data, uint32_t sizeInBytes, uint32_t bufferOffsetInBytes = 0) override;

	};


}