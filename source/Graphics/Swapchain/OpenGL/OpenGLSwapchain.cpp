// Monocle Game Engine source files - Alexandre Baron

#include "OpenGLSwapchain.h"

#include "Graphics/Device/OpenGL/OpenGLGraphicsDevice.h"

#include "Core/Misc/Literals.h"

#include "glad/glad.h"

namespace moe
{
	void OpenGLSwapchain::Create(IGraphicsDevice& /*device*/, uint32_t /*renderWidth*/, uint32_t /*renderHeight*/, FramebufferAttachment /*wantedAttachments*/)
	{
		//OpenGLGraphicsDevice& glDevice = static_cast<OpenGLGraphicsDevice&>(device);

		//// First create the swap chain's framebuffer
		//m_mainFramebuffer.Create();
	}
}
