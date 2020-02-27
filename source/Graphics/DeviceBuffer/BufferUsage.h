// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"

#include "Core/Misc/moeEnum.h"

namespace moe
{
	/*
		This enum is a bitmask specifying the intended use of a DeviceBuffer (vertex buffer, index buffer, etc.)
		This is partly inspired by the Veldrid API.
	*/
	MOE_ENUM(BufferUsage, byte_t,
		None = 0,
		// Indicates that a DeviceBuffer can be used as the source of vertex data for drawing commands.
		VertexBuffer = 1 << 0,
		// Indicates that a DeviceBuffer can be used as the source of index data for drawing commands.
		IndexBuffer = 1 << 1,
		// Indicates that a DeviceBuffer can be used as a uniform Buffer. This flag enables the use of a Buffer in a ResourceSet as a uniform Buffer.
		UniformBuffer = 1 << 2,
		// Indicates that a DeviceBuffer can be used as a read-only structured Buffer. Enables buffer usage in a ResourceSet as a read-only structured buffer.
		// This flag can only be combined with Dynamic.
		StructuredBufferReadOnly = 1 << 3,
		// Indicates that a DeviceBuffer can be used as a read-only structured Buffer. Enables buffer usage in a ResourceSet as a read-only structured buffer.
		// This flag cannot be combined with any other flag.
		StructuredBufferReadWrite = 1 << 4,
		// Indicates that a DeviceBuffer can be used as the source of indirect drawing information. Enables buffer usage by the Indirect* methods of CommandList.
		// This flag cannot be combined with Dynamic.
		IndirectBuffer = 1 << 5,
		// Indicates that a DeviceBuffer will be updated with new data very frequently. Can be mapped with Write.
		Dynamic = 1 << 6,
		// Indicates that a DeviceBuffer will be used as a staging Buffer. Staging Buffers can be used to transfer data to and from the CPU using Map.
		// Staging Buffers can use all Map modes. This flag cannot be combined with any other flag.
		Staging = 1 << 7

		// There is no "Static" flag to keep it in a byte. but obviously a Static buffer is used for static geometry. It is Static if neither of Dynamic or Staging bits are set.
	);
	DECLARE_MOE_ENUM_OPERATORS(BufferUsage);


}