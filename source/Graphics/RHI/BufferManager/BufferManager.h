#pragma once
#include "Core/Resource/MeshResource.h"
#include "Core/Resource/Material/MaterialResource.h"
#include "Graphics/DeviceBuffer/DeviceBufferHandle.h"

namespace moe
{
	struct RenderMeshHandle
	{
		DeviceBufferHandle	VertexBuffer = DeviceBufferHandle::Null();
		DeviceBufferHandle	IndexBuffer = DeviceBufferHandle::Null();
	};

	struct MeshData;
	/*
	 * An abstract class for device buffer (vertex, index, uniform -AKA constant-, storage -aka structured- ...) buffer management.
	 */
	class IBufferManager
	{
	public:

		virtual ~IBufferManager() = default;

		virtual RenderMeshHandle	FindOrCreateMeshBuffer(Ref<MeshResource> const& meshRsc) = 0;

		virtual RenderMeshHandle	FindOrCreateMeshBuffer(MeshData const& meshData) = 0;

		virtual void				DestroyDeviceBuffer(DeviceBufferHandle bufferToDestroy) = 0;
	};


}
