#pragma once

#include "BufferManager/BufferManager.h"
#include "FramebufferManager/FramebufferManager.h"
#include "PipelineManager/PipelineManager.h"
#include "TextureManager/TextureManager.h"
#include "SwapchainManager/SwapchainManager.h"
#include "MaterialManager/MaterialManager.h"

namespace moe
{
	class CommandBuffer;
	/*
	 * A rendering API-agnostic abstraction of render hardware interface.
	 * You need to implement this RHI with every rendering API you use in order to use it with a Renderer.
	 * The RHI covers the following data types :
	 * - buffers (vertex, index, uniform...)
	 * - pipeline state objects (PSO)
	 * - textures and render targets
	 * - render passes
	 * - shaders
	 * - more ?...
	 */
	class RenderHardwareInterface
	{
	public:

		virtual ~RenderHardwareInterface() = default;

		virtual void					SubmitCommandBuffer(CommandBuffer const& cmdBuf) = 0;

		virtual	IPipelineManager&		PipelineManager() = 0;

		virtual IBufferManager&			BufferManager() = 0;

		virtual ITextureManager&		TextureManager() = 0;

		virtual IFramebufferManager&	FramebufferManager() = 0;

		virtual ISwapchainManager&		SwapchainManager() = 0;

		virtual IMaterialManager&		MaterialManager() = 0;

	};


}