#pragma once
#include "Graphics/CommandBuffer/CommandBuffer.h"

namespace moe
{
	enum class TranslucencyType : uint8_t
	{
		Opaque = 0,
		Additive = 0b1,
		Subtractive = 0b10,
		AlphaBlended = 0b11
	};

	enum class ViewportLayer : uint8_t
	{
		LAST = 0,
		HUD = LAST, // HUD is always the last because we never want to draw something on top of it
		GUI = HUD,
		Skybox = 1, // Skybox is always drawn last in the world to profit from depth testing
		World
	};


#define PASS_BITS 8

#define VIEWPORT_BITS 3

	struct RenderQueueKey
	{
		uint64_t	Pass		: PASS_BITS;
		uint64_t	ViewLayer	: 3;
		uint64_t	Cmd			: 3;
		uint64_t	Program		: 16;
		uint64_t	Material	: 16;
		uint64_t	Depth		: 9;
		uint64_t	Translucent : 3;
		uint64_t	ViewportID	: VIEWPORT_BITS;
		uint64_t	Fullscreen	: 3;

		inline static const int MAX_VIEWPORTS = 1 << VIEWPORT_BITS;
		inline static const int MAX_PASSES = 1 << PASS_BITS;
	};

	static_assert(sizeof(RenderQueueKey) == sizeof(uint64_t));

	// Stolen from https://aras-p.info/blog/2014/01/16/rough-sorting-by-depth/
	// Taking highest 10 bits for rough sort of positive floats.
	// Sign is always zero, so only 9 bits in the result are used.
	// 0.01 maps to 240; 0.1 to 247; 1.0 to 254; 10.0 to 260;
	// 100.0 to 267; 1000.0 to 273 etc.
	inline unsigned DepthToBits(float depth)
	{
		union { float f; unsigned i; } f2i;
		f2i.f = depth;
		unsigned b = f2i.i >> 22; // take highest 10 bits
		return b;
	}


	struct RenderQueueIndex
	{
		RenderQueueIndex(uint32_t idx, RenderQueueKey k) :
			Index(idx), Key(k)
		{}

		uint32_t		Index{ 0 };
		RenderQueueKey	Key{};
	};



	class RenderQueue
	{
	public:

		void	Sort();

		template <typename TCmd, typename... Args>
		void	EmplaceDrawCall(RenderQueueKey key, Args&&... args)
		{
			m_indices.EmplaceBack((uint32_t)m_cmdBuffer.Size(), key);
			m_cmdBuffer.Emplace<TCmd, Args...>(std::forward<Args>(args)...);
		}

		template <typename TCmd, typename... Args>
		RenderQueueKey	EmplaceCommand(RenderQueueKey key, Args&&... args)
		{
			MOE_ASSERT(key.Cmd < 0b111);
			key.Cmd++;
			EmplaceDrawCall<TCmd, Args...>(key, std::forward<Args>(args)...);
			return key;
		}


		CommandBuffer	const&	GetCommandBuffer() const
		{
			return m_cmdBuffer;
		}



		static RenderQueueKey ComputeRenderQueueKey(uint8_t passIndex, uint32_t cmd = 0, uint32_t program = 0, uint32_t matID = 0,
		                                            float Depth = 0,
		                                            TranslucencyType translut = TranslucencyType::Opaque,
		                                            ViewportLayer layer = ViewportLayer::World, uint8_t viewportID = 0,
		                                            uint8_t fullscreen = 0);


	private:

		CommandBuffer				m_cmdBuffer;

		Vector<RenderQueueIndex>	m_indices;

	};

	inline RenderQueueKey RenderQueue::ComputeRenderQueueKey(uint8_t passIndex, uint32_t cmd, uint32_t program, uint32_t matID, float Depth,
		TranslucencyType translut, ViewportLayer layer,
		uint8_t viewportID, uint8_t fullscreen)
	{
		MOE_ASSERT(
			program <= moe::MaxValue<uint16_t>() && matID <= moe::MaxValue<uint16_t>() && viewportID <= 8 && fullscreen
			<= 8);
		RenderQueueKey key;
		key.Pass = passIndex;
		key.Cmd = cmd;
		key.Program = program;
		key.Material = matID;
		key.Depth = DepthToBits(Depth);
		key.Translucent = (uint8_t)translut;
		key.ViewLayer = (uint8_t)layer;
		key.ViewportID = viewportID;
		key.Fullscreen = fullscreen;
		return key;
	}
}
