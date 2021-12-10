#pragma once
#include <filesystem>

#include "BaseResource.h"
#include "Core/Containers/Array/Array.h"
#include "Core/Containers/Vector/Vector.h"
#include "Core/Preprocessor/moeAssert.h"


namespace moe
{

	// Needed for Core not to depend on graphics, Matches the Vulkan values for now (TODO: Clean up this)
	enum class MeshIndexType : uint8_t
	{
		eUint16 = 0,
		eUint32
	};

	struct MeshData
	{
	private:

		using RawData = Vector<std::byte>;

		std::byte const* ToBytes(void const* anything)
		{
			return reinterpret_cast<std::byte const*>(anything);
		}

	public:
		MeshData() = default;

		MeshData(void const* vtxData, size_t vtxSize, size_t numVerts, void const* idxData, size_t idxSize, size_t numIndices) :
			Vertices(ToBytes(vtxData), ToBytes(vtxData) + (vtxSize * numVerts)),
			Indices(ToBytes(idxData), ToBytes(idxData) + (idxSize * numIndices)),
			MeshIndexType((idxSize == sizeof(uint32_t) ? MeshIndexType::eUint32 : MeshIndexType::eUint16))
		{
			MOE_ASSERT(idxSize == sizeof(uint32_t) || idxSize == sizeof(uint16_t));
		}

		template <template <typename> class VtxContainer, typename VertexType>
		static MeshData	Build(VtxContainer<VertexType> & vertices)
		{
			return MeshData(vertices.Data(), sizeof(VertexType), vertices.Size(), nullptr, 0, 0);
		}

		template <typename VertexType, size_t N>
		static MeshData	Build(Array<VertexType, N> const& vertices)
		{
			return MeshData(vertices.Data(), sizeof(VertexType), N, nullptr, sizeof(uint16_t), 0);
		}


		RawData			Vertices{};
		RawData			Indices{};
		MeshIndexType	MeshIndexType{ MeshIndexType::eUint32 };
	};


	class MeshResource : public IBaseResource
	{
	public:

		MeshResource() = default;

		MeshResource(MeshData&& meshData) :
			Data(std::move(meshData))
		{}

		~MeshResource() override = default;

		MeshData	Data;
	};


}
