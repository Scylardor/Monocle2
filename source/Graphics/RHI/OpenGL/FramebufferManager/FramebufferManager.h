#pragma once
#include <cstdint>
#include <glad/glad.h>



#include "Graphics/Handle/ObjectHandle.h"

#include "Monocle_Graphics_Export.h"

#include "Graphics/RHI/FramebufferManager/FramebufferManager.h"

namespace moe
{
	class OpenGL4TextureManager;

	struct OpenGL4FramebufferDescription : FramebufferDescription
	{
		OpenGL4FramebufferDescription(GLuint id, int w = 0, int h = 0, uint32_t s = 1) :
			FramebufferDescription(w, h, s),
			FramebufferID(id)
		{}

		OpenGL4FramebufferDescription(GLuint id, std::pair<int, int> const& wh, uint32_t s = 1) :
			FramebufferDescription(wh, s),
			FramebufferID(id)
		{}

		GLuint	FramebufferID{ 0 };
	};

	/*
	 * An abstract class for render targets (sometimes AKA renderbuffers) and framebuffer management.
	 */
	class OpenGL4FramebufferManager : public IFramebufferManager
	{
	public:

		OpenGL4FramebufferManager(OpenGL4TextureManager& textureManager) :
			m_texManager(textureManager)
		{}

		DeviceFramebufferHandle	CreateFramebuffer(std::pair<int, int> const& dimensions, uint32_t numSamples = 1) override;

		DeviceTextureHandle	CreateFramebufferColorAttachment(DeviceFramebufferHandle fbHandle, TextureFormat format = TextureFormat::RGB32F, TextureUsage usage = TextureUsage(Sampled | RenderTarget)) override;

		DeviceTextureHandle	CreateDepthStencilAttachment(DeviceFramebufferHandle fbHandle, TextureFormat format = TextureFormat::Depth24_Stencil8, TextureUsage usage = TextureUsage(DepthStencil | RenderTarget)) override;

	private:

		OpenGL4FramebufferDescription&	MutFramebuffer(DeviceFramebufferHandle fbHandle)
		{
			// should be ok to cast to uint32 : it's supposed to be an object pool ID
			return m_frameBuffers.Mut((uint32_t) fbHandle.Get());
		}


		OpenGL4TextureManager&								m_texManager;

		DynamicObjectPool<OpenGL4FramebufferDescription>	m_frameBuffers{};
	};


}
