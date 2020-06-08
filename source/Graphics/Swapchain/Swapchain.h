// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"
#include "Graphics/Framebuffer/Framebuffer.h"
#include "Graphics/Framebuffer/FramebufferAttachments.h"

#include "Graphics/Device/GraphicsDevice.h"

namespace moe
{

	/**
	 * \brief A graphics API-agnostic structure describing a swap chain.
	 * It provides framebuffer creation and present capabilities.
	 * Use it to manage the render targets currently in use by the renderer.
	 */
	class ASwapchain
	{
	public:
		virtual ~ASwapchain() = default;


		[[nodiscard]] virtual const AFramebuffer&	GetFrameBuffer() const = 0;

	protected:
		virtual void	Create(IGraphicsDevice& device, uint32_t renderWidth, uint32_t renderHeight, FramebufferAttachment wantedAttachments) = 0;

	};

}