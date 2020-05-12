// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_STD_SUPPORT
#include <string>
#endif

#include "Core/Containers/Vector/Vector.h"

#include "Graphics/DeviceBuffer/DeviceBufferHandle.h"

namespace moe
{
	class Material;
	class RenderWorld;
	class AGraphicObject;


	struct UniformBlockLocation
	{
		DeviceBufferHandle	m_bufferHandle{ 0 };
		uint32_t			m_blockOffset{ 0 };
	};

	class IMaterialObjectBlock
	{
	public:
		virtual ~IMaterialObjectBlock() = default;

		virtual void	OnObjectDraw(RenderWorld& currentWorld, Material& currentMaterial, AGraphicObject& object, const UniformBlockLocation& blockLoc) = 0;

		[[nodiscard]] virtual uint32_t	GetObjectBlockDataSize() const = 0;

		[[nodiscard]] virtual std::string	GetObjectBlockName() const = 0;
	};

}