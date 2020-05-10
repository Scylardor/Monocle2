// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"

#include "Graphics/Handle/ObjectHandle.h"

#include "Monocle_Graphics_Export.h"


namespace moe
{

	struct Monocle_Graphics_API UniformBufferHandle : RenderObjectHandle<std::uint64_t>
	{
		static UniformBufferHandle	Null() { return UniformBufferHandle{ 0 }; }

	};

}

namespace std {

	template <>
	struct hash<moe::UniformBufferHandle>
	{
		std::size_t operator()(const moe::UniformBufferHandle& k) const noexcept
		{
			return hash<moe::UniformBufferHandle::Underlying>()(k.Get());
		}
	};

}