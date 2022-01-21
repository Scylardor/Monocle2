// Monocle source files - Alexandre Baron

#pragma once
#include "Core/Containers/Vector/Vector.h"
#include "Graphics/Color/Color.h"
#include "Graphics/RenderQueue/RenderQueue.h"
#include "Graphics/RHI/FramebufferManager/FramebufferManager.h"

namespace moe
{
	class Renderer;
	class RenderQueue;
	class RenderHardwareInterface;


	class ISubpass
	{
	public:

		virtual ~ISubpass() = default;

		virtual RenderQueueKey	Update(RenderQueue& drawQueue, RenderQueueKey key, int subpassIdx) = 0;

	};

	class IRenderPass
	{
	public:

		IRenderPass() = default;


		/* Whether the render pass resizes its framebuffer at the same time as the renderer's surface or not. */
		enum class SurfaceDimensionsSync
		{
			Yes,
			No
		};

		IRenderPass(Renderer& owner, SurfaceDimensionsSync isResizingSynchronized = SurfaceDimensionsSync::Yes);

		virtual ~IRenderPass() = default;

		virtual RenderQueueKey	Update(RenderQueue & drawQueue, RenderQueueKey key)
		{
			key = drawQueue.EmplaceCommand<CmdBeginRenderPass>(key, RenderPassFramebuffer(), PassClearColor());

			key = UpdateSubpasses(drawQueue, key);

			key = drawQueue.EmplaceCommand<CmdEndRenderPass>(key);

			return key;
		}

		template <typename T, typename... Ts>
		T*	EmplaceSubpass(Ts&&... subpassArgs)
		{
			static_assert(std::is_base_of_v<ISubpass, T>);
			m_subPasses.EmplaceBack(std::make_unique<T>(std::forward<Ts>(subpassArgs)...));
			return static_cast<T*>(m_subPasses.Back().get());
		}


		void	RemoveSubpass(ISubpass* subpass)
		{
			auto it = std::find_if(m_subPasses.begin(), m_subPasses.end(), [subpass](auto& subpassPtr)
				{
					return subpassPtr.get() == subpass;
				});

			if (it != m_subPasses.end())
				m_subPasses.EraseBySwap(it);
		}


		void	Destroy(RenderHardwareInterface& rhi);

		[[nodiscard]] DeviceFramebufferHandle	RenderPassFramebuffer() const
		{
			return m_framebuffer;
		}

		[[nodiscard]] ColorRGBAf const&	PassClearColor() const
		{
			return m_FBClearColor;
		}



	protected:

		RenderQueueKey	UpdateSubpasses(RenderQueue& drawQueue, RenderQueueKey key)
		{
			int subpassIdx = 0;
			for (auto& subpassPtr : m_subPasses)
			{
				key = subpassPtr->Update(drawQueue, key, subpassIdx);
				subpassIdx++;
			}

			return key;
		}


		void	CreateFramebuffer(Renderer& renderer, std::pair<int, int> const& dimensions);


		void	SetFramebuffer(DeviceFramebufferHandle fbHandle)
		{
			m_framebuffer = fbHandle;
		}


		void	SetClearColor(ColorRGBAf const& clearColor)
		{
			m_FBClearColor = clearColor;
		}

	private:

		void	CreateSurfaceSynchronizedFramebuffer();

		void	OnGraphicsSurfaceResized(int newWidth, int newHeight);

		Renderer*							m_renderer = nullptr;
		Vector<std::unique_ptr<ISubpass>>	m_subPasses;
		DeviceFramebufferHandle				m_framebuffer{};
		ColorRGBAf							m_FBClearColor{ ColorRGBAf::Black() };
	};



}
