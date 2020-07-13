// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Containers/Vector/Vector.h"
#include "Graphics/RenderTarget/RenderTargetHandle.h"


namespace moe
{

	// TODO: put this elsewhere (used for framebuffer blitting)
	template <typename T>
	struct Rect2D
	{
		T x{0};
		T y{0};
		T width{0};
		T height{0};
	};

	typedef	Rect2D<int>	Rect2Di;


	/**
	 * \brief A graphics API-agnostic class implementing a Framebuffer.
	 * A framebuffer is basically a holder of render targets you can use for rendering.
	 * It has the notion of "completeness" and using it is valid only it is complete.
	 * The requirements for a valid framebuffer are :
	 * - It needs to have at least one attachment (color, depth or stencil buffer).
	 * - There should be at least one color attachment.
	 * - All attachments should be complete as well (reserved memory).
	 * - Each attachment should have the same number of samples.
	 */
	class AFramebuffer
	{
	public:

		AFramebuffer() = default;

		virtual ~AFramebuffer() = default;

		virtual void	Bind() = 0;
		virtual void	Unbind() = 0;

		virtual bool	IsComplete() const = 0;

	private:
		Vector<RenderTargetHandle>	m_colorAttachments;
		RenderTargetHandle			m_depthStencilAttachment;
	};

}