#pragma once
#include <cstdint>

#include "Graphics/Handle/ObjectHandle.h"

#include "Monocle_Graphics_Export.h"

#include "Graphics/RHI/FramebufferManager/FramebufferManager.h"

namespace moe
{
	enum class TextureFormat : unsigned char;


	/*
	 * An abstract class for render targets (sometimes AKA renderbuffers) and framebuffer management.
	 */
	class OpenGL4FramebufferManager : public IFramebufferManager
	{
	public:

		~OpenGL4FramebufferManager() override = default;

		DeviceFramebufferHandle	CreateFramebuffer() override;

		DeviceFramebufferHandle	CreateFramebufferColorAttachment(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGB8) override;

		DeviceFramebufferHandle	CreateDepthStencilAttachment(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::Depth24_Stencil8) override;
	};


}
