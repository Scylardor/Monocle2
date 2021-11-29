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