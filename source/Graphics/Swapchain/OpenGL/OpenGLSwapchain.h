// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/Swapchain/Swapchain.h"

#include "Graphics/Framebuffer/FramebufferHandle.h"

#include "Graphics/Framebuffer/OpenGL/OpenGLFramebuffer.h"

namespace moe
{

	/**
	 * \brief OpenGL implementation of a swap chain.
	 * Swap chains don't really exist in classic OpenGL (or are hidden inside the GL context).
	 * It tries to mimic the workings of an actual swap chain by allowing us to manipulate custom-made framebuffers.
	 */
	class OpenGLSwapchain final : public ASwapchain
	{
	public:
		OpenGLSwapchain() = default;


		OpenGLSwapchain(const OpenGLSwapchain& other) = delete;
		OpenGLSwapchain& operator=(const OpenGLSwapchain& rhs) = delete;

		OpenGLSwapchain(OpenGLSwapchain&& other) noexcept
		{
			m_mainFramebuffer = std::move(other.m_mainFramebuffer);
		}

		OpenGLSwapchain& OpenGLFrameBuffer(OpenGLSwapchain&& rhs)
		{
			if (&rhs != this)
			{
				m_mainFramebuffer = std::move(rhs.m_mainFramebuffer);
			}
			return *this;
		}


		void	Create(IGraphicsDevice& device, uint32_t renderWidth, uint32_t renderHeight, FramebufferAttachment wantedAttachments) override;

		[[nodiscard]] virtual const AFramebuffer&	GetFrameBuffer() const override
		{
			return m_mainFramebuffer;
		}

	private:

		OpenGLFramebuffer	m_mainFramebuffer;
	};

}