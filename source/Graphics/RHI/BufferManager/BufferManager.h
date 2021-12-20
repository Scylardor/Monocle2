#pragma once
#include "Core/Resource/MeshResource.h"
#include "Core/Resource/Material/MaterialResource.h"
#include "Graphics/DeviceBuffer/DeviceBufferHandle.h"

namespace moe
{

	struct DeviceBufferMapping
	{
		static const auto WHOLE_RANGE = -1;

		DeviceBufferMapping() = default;

		DeviceBufferMapping(ObjectPoolID id, DeviceBufferHandle handle, void* dat) :
			ID(id), BufferHandle(handle), Data(dat)
		{}

		template <typename T>
		T As()
		{
			return static_cast<T>(Data);
		}

		[[nodiscard]] auto	MappingID() const
		{
			return ID;
		}

		auto	Handle() const
		{
			return BufferHandle;
		}

	private:

		ObjectPoolID		ID{ INVALID_ID };
		DeviceBufferHandle	BufferHandle{};
		void*				Data = nullptr;
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

		virtual DeviceBufferMapping	MapCoherentDeviceBuffer(size_t dataSize, void const* data = nullptr,
			uint32_t mappingOffset = 0, size_t mappingRange = DeviceBufferMapping::WHOLE_RANGE) = 0;

		virtual void				Unmap(DeviceBufferMapping const& bufferMap) = 0;
		virtual void				ResizeMapping(DeviceBufferMapping & bufferMap, uint32_t newSize) = 0;


		virtual void				DestroyDeviceBuffer(DeviceBufferHandle bufferToDestroy) = 0;
	};


}
