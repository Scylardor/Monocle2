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

	struct DeviceMeshHandle : RenderableObjectHandle<std::uint32_t>
	{
	private:
		static const Underlying ms_INVALID_ID = (Underlying) -1;

	public:

		DeviceMeshHandle(Underlying handleID = ms_INVALID_ID) :
			RenderableObjectHandle(handleID)
		{}

		static DeviceMeshHandle	Null() { return DeviceMeshHandle(); }


	};

	struct MeshData;
	/*
	 * An abstract class for device buffer (vertex, index, uniform -AKA constant-, storage -aka structured- ...) buffer management.
	 */
	class IBufferManager
	{
	public:

		virtual ~IBufferManager() = default;

		virtual DeviceMeshHandle	FindOrCreateMeshBuffer(Ref<MeshResource> const& meshRsc) = 0;

		virtual DeviceMeshHandle	FindOrCreateMeshBuffer(MeshData const& meshData) = 0;

		virtual void				DestroyDeviceBuffer(DeviceBufferHandle bufferToDestroy) = 0;
	};


}
