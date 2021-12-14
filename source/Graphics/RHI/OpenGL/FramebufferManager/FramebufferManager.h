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
	class OpenGL4FramebufferManager final : public IFramebufferManager
	{
	public:

		OpenGL4FramebufferManager(OpenGL4TextureManager& textureManager) :
			m_texManager(textureManager)
		{}

		[[nodiscard]] DeviceFramebufferHandle	CreateFramebuffer(std::pair<int, int> const& dimensions, uint32_t numSamples = 1) override;

		DeviceTextureHandle	CreateFramebufferColorAttachment(DeviceFramebufferHandle fbHandle, TextureFormat format = TextureFormat::RGB32F, TextureUsage usage = TextureUsage(Sampled | RenderTarget)) override;

		DeviceTextureHandle	CreateDepthStencilAttachment(DeviceFramebufferHandle fbHandle, TextureFormat format = TextureFormat::Depth24_Stencil8, TextureUsage usage = TextureUsage(DepthStencil | RenderTarget)) override;

		void	BindFramebuffer(DeviceFramebufferHandle fbHandle, TargetBuffer readBuffer = TargetBuffer::Default, TargetBuffer writeBuffer = TargetBuffer::Default) override;

		void	DestroyFramebuffer(DeviceFramebufferHandle fbHandle, DestroyAttachmentMode attachMode = DestroyAttachmentMode::DestroyAttachments) override;

		void	UnbindFramebuffer(DeviceFramebufferHandle fbHandle) override;
		void	UnbindFramebuffer();
		void	DestroyFramebuffer(DeviceFramebufferHandle fbHandle);
		void	ResizeFramebuffer(DeviceFramebufferHandle fbHandle, std::pair<int, int> const& dimensions);

		void	AddColorAttachment(DeviceFramebufferHandle fbHandle, DeviceTextureHandle texHandle) override;
		void	SetDepthStencilAttachment(DeviceFramebufferHandle fbHandle, DeviceTextureHandle texHandle) override;


		[[nodiscard]] DeviceTextureHandle	GetFramebufferColorAttachment(DeviceFramebufferHandle fbHandle, uint32_t colorAttachmentIdx = 0);
		[[nodiscard]] DeviceTextureHandle	GetFramebufferDepthStencilAttachment(DeviceFramebufferHandle fbHandle);


		[[nodiscard]] OpenGL4FramebufferDescription const& GetFramebuffer(DeviceFramebufferHandle fbHandle)
		{
			// should be ok to cast to uint32 : it's supposed to be an object pool ID
			return m_frameBuffers.Get((uint32_t)fbHandle.Get());
		}

	private:


		[[nodiscard]] OpenGL4FramebufferDescription&	MutFramebuffer(DeviceFramebufferHandle fbHandle)
		{
			// should be ok to cast to uint32 : it's supposed to be an object pool ID
			return m_frameBuffers.Mut((uint32_t) fbHandle.Get());
		}

		void			BindAttachment(GLint framebufferID, unsigned int attachmentID, GLuint textureID, TextureUsage usage, int mipLevel = 0, bool layered = false, int layerIdx = 0);

		void			BindDepthStencilAttachment(GLint framebufferID, DeviceTextureHandle attachmentHandle);

		[[nodiscard]] static GLint	GetMaxColorAttachments();


		OpenGL4TextureManager&								m_texManager;

		DynamicObjectPool<OpenGL4FramebufferDescription>	m_frameBuffers{};
	};


}
