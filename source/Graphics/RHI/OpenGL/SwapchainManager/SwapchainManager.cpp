#include "SwapchainManager.h"

#include "Graphics/RHI/OpenGL/OGL4RHI.h"

#include "GameFramework/Service/RenderService/GraphicsSurface/GraphicsSurface.h"

namespace moe
{

	DeviceSwapchainHandle OpenGL4SwapchainManager::CreateSwapchain(RenderHardwareInterface* RHI, IGraphicsSurface* boundSurface)
	{
		auto* rhi = dynamic_cast<OpenGL4RHI*>(RHI);
		if (rhi == nullptr)
		{
			MOE_FATAL(ChanGraphics, "OpenGL swapchain manager not used with OpenGL RHI : something is very wrong.");
			MOE_ASSERT(false);
			return DeviceSwapchainHandle::Null();
		}

		MOE_ASSERT(boundSurface != nullptr);
		if (boundSurface == nullptr)
		{
			MOE_ERROR(ChanGraphics, "OpenGL swapchain manager needs to have a valid graphics surface to create a swapchain.");
			return DeviceSwapchainHandle::Null();
		}

		auto [width, height] = boundSurface->GetDimensions();

		auto scID = m_swapchains.Emplace(boundSurface, width, height);
		auto swapChainHandle = DeviceSwapchainHandle(scID);

		//using namespace std::placeholders;
		//auto resizeDelegate = std::bind(this, &OpenGL4SwapchainManager::OnSurfaceResized, swapChainHandle, _1, _2);

		//IGraphicsSurface::SurfaceResizedEvent::DelegateType
		//boundSurface->OnSurfaceResizedEvent().AddDelegate((resizeDelegate);


		return swapChainHandle;
	}


	bool OpenGL4SwapchainManager::Present(DeviceSwapchainHandle presentedSwapchain)
	{
		// OpenGL has no swapchain per se, and is 100% relying on the actual graphics surface to swap buffers.
		OpenGLSwapchain& swapchain = m_swapchains.Mut(presentedSwapchain.Get());

		swapchain.Surface->SwapBuffers();

		return true;
	}

	void	OpenGL4SwapchainManager::OnSurfaceResized(DeviceSwapchainHandle swapchainHandle, int width, int height)
	{
		swapchainHandle;
		width;
		height;
	}
}
