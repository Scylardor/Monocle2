// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"

#include "Graphics/Handle/ObjectHandle.h"

#include "Monocle_Graphics_Export.h"


namespace moe
{

	/**
	 * \brief A generic graphics-API agnostic handle to a buffer of any kind of GPU data.
	 */
	struct Monocle_Graphics_API DeviceBufferHandle : RenderableObjectHandle<std::uint64_t>
	{
		// TODO: these functions are mostly here for convenience now, for easier interop with OpenGL
		// But maybe we can generalize the "openGL way" to any API ?
		[[nodiscard]] uint32_t	DecodeBufferID() const
		{
			uint64_t handleVal = Get();
			uint32_t bufferID = handleVal >> 32;
			return  bufferID;
		}


		[[nodiscard]] uint32_t DecodeBufferOffset() const
		{
			uint32_t offset = (uint32_t)Get();
			return offset;
		}


		[[nodiscard]] std::pair<unsigned, unsigned> DecodeBufferHandle() const
		{
			uint64_t handleVal = Get();
			uint32_t bufferID = handleVal >> 32;
			uint32_t bufferOffset = (uint32_t)handleVal;
			return { bufferID, bufferOffset };
		}

		static DeviceBufferHandle Encode(uint32_t bufferID, uint32_t bufferOffset)
		{
			uint64_t handleValue = (uint64_t)bufferID << 32;
			handleValue |= bufferOffset;
			return DeviceBufferHandle{ handleValue };
		}

		static DeviceBufferHandle	Null() { return DeviceBufferHandle{ 0 }; }
	};



}



namespace std
{

	template <>
	struct hash<moe::DeviceBufferHandle>
	{
		std::size_t operator()(const moe::DeviceBufferHandle& k) const noexcept
		{
			return hash<moe::DeviceBufferHandle::Underlying>()(k.Get());
		}
	};

}