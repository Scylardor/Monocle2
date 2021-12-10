#pragma once
#include "Graphics/Handle/ObjectHandle.h"

#include "Monocle_Graphics_Export.h"

#include "Graphics/Texture/TextureFormat.h"

#include "Core/Containers/Vector/Vector.h"

#include <cstdint>

#include "Graphics/Framebuffer/FramebufferDescription.h"
#include "Graphics/RHI/TextureManager/TextureManager.h"

namespace moe
{

	struct Monocle_Graphics_API	DeviceFramebufferHandle : RenderableObjectHandle<std::uint64_t>
	{
	private:
		static const std::uint64_t ms_INVALID_ID = (std::uint64_t) -1;

	public:

		DeviceFramebufferHandle() = default;
		DeviceFramebufferHandle(std::uint64_t handleID) :
			RenderableObjectHandle(handleID)
		{}

		static DeviceFramebufferHandle	Null() { return DeviceFramebufferHandle(); }

	};


	/**
	 * \brief A graphics APi-agnostic descriptor of what textures, or attachments, should be used to populate a framebuffer.
	 */
	struct FramebufferDescription
	{
		FramebufferDescription(int w = 0, int h = 0, uint32_t s = 1) :
			Width(w), Height(h), Samples(s)
		{}

		FramebufferDescription(std::pair<int,int> const& wh, uint32_t s = 1) :
			Width(wh.first), Height(wh.second), Samples(s)
		{}

		int							Width{ 0 };
		int							Height{ 0 };
		uint32_t					Samples{ 1 };
		Vector<DeviceTextureHandle>	ColorAttachments;
		DeviceTextureHandle			DepthStencilAttachment{DeviceTextureHandle::Null()};

		// The source color buffer to use for all read operations when this framebuffer is bound.
		TargetBuffer				ReadBuffer{ TargetBuffer::Default };

		// The destination color buffer to use for all write operations when this framebuffer is bound.
		TargetBuffer				DrawBuffer{ TargetBuffer::Default };
	};



	/*
	 * An abstract class for render targets (sometimes AKA renderbuffers) and framebuffer management.
	 */
	class IFramebufferManager
	{
	public:

		virtual ~IFramebufferManager() = default;

		virtual DeviceFramebufferHandle	CreateFramebuffer(std::pair<int, int> const& dimensions, uint32_t numSamples = 1) = 0;

		virtual DeviceTextureHandle	CreateFramebufferColorAttachment(DeviceFramebufferHandle fbHandle, TextureFormat format = TextureFormat::RGB32F, TextureUsage usage = TextureUsage(Sampled | RenderTarget)) = 0;

		virtual DeviceTextureHandle	CreateDepthStencilAttachment(DeviceFramebufferHandle fbHandle, TextureFormat format = TextureFormat::Depth24_Stencil8, TextureUsage usage = TextureUsage(DepthStencil | RenderTarget)) = 0;

		virtual void	AddColorAttachment(DeviceFramebufferHandle fbHandle, DeviceTextureHandle texHandle) = 0;
		virtual void	SetDepthStencilAttachment(DeviceFramebufferHandle fbHandle, DeviceTextureHandle texHandle) = 0;

		virtual void	BindFramebuffer(DeviceFramebufferHandle fbHandle, TargetBuffer readBuffer = TargetBuffer::Default, TargetBuffer writeBuffer = TargetBuffer::Default) = 0;

		virtual void	UnbindFramebuffer(DeviceFramebufferHandle fbHandle) = 0;
	};


}
