#pragma once
#include "Graphics/Handle/ObjectHandle.h"

#include "Monocle_Graphics_Export.h"

#include "Graphics/Texture/TextureFormat.h"

#include "Core/Containers/Vector/Vector.h"

#include <cstdint>

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


	struct RenderTargetDescription
	{
		uint32_t	Width{ 0 };
		uint32_t	Height{ 0 };



	};


	struct FramebufferDescription
	{
		Vector<RenderTargetDescription>	TargetDescriptions{};

	};



	/*
	 * An abstract class for render targets (sometimes AKA renderbuffers) and framebuffer management.
	 */
	class IFramebufferManager
	{
	public:

		virtual ~IFramebufferManager() = default;

		virtual DeviceFramebufferHandle	CreateFramebuffer() = 0;

		virtual DeviceFramebufferHandle	CreateFramebufferColorAttachment(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGB8) = 0;

		virtual DeviceFramebufferHandle	CreateDepthStencilAttachment(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::Depth24_Stencil8) = 0;
	};


}
