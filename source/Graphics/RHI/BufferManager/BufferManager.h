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

		DeviceBufferMapping(ObjectPoolID id, DeviceBufferHandle handle, void* dat, uint32_t blockSize) :
			ID(id), BufferHandle(handle), Data(dat), BlockSize(blockSize)
		{}

		template <typename T>
		T& MutBlock(uint32_t idx = 0)
		{
			auto* ptr = (char*)Data;
			ptr += (BlockSize * idx);
			return *reinterpret_cast<T*>(ptr);
		}

		template <typename T>
		T const& GetBlock(uint32_t idx = 0) const
		{
			auto* ptr = (char const*)Data;
			ptr += (BlockSize * idx);
			return *reinterpret_cast<T const*>(ptr);
		}

		[[nodiscard]] auto	MappingID() const
		{
			return ID;
		}

		[[nodiscard]] auto	Handle() const
		{
			return BufferHandle;
		}

		[[nodiscard]] auto	AlignedBlockSize() const
		{
			return BlockSize;
		}

	private:

		ObjectPoolID		ID{ INVALID_ID };
		DeviceBufferHandle	BufferHandle{};
		void*				Data = nullptr;
		uint32_t			BlockSize = 0;
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

		virtual DeviceBufferMapping	MapCoherentDeviceBuffer(uint32_t dataBlockSize, uint32_t numBlocks = 1, void const* data = nullptr,
			bool alignUniform = true, uint32_t mappingOffset = 0, size_t mappingRange = DeviceBufferMapping::WHOLE_RANGE) = 0;

		virtual void				Unmap(DeviceBufferMapping const& bufferMap) = 0;
		virtual void				ResizeMapping(DeviceBufferMapping & bufferMap, uint32_t newSize) = 0;


		virtual void				DestroyDeviceBuffer(DeviceBufferHandle bufferToDestroy) = 0;
	};


}
