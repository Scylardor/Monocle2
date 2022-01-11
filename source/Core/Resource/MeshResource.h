#pragma once
#include <filesystem>

#include "BaseResource.h"
#include "Core/Containers/Array/Array.h"
#include "Core/Containers/Vector/Vector.h"
#include "Core/Preprocessor/moeAssert.h"


namespace moe
{

	// Needed for Core not to depend on graphics, Matches the Vulkan values for now (TODO: Clean up this)
	enum class MeshElementType : uint32_t
	{
		eUint16 = 0,
		eUint32,
		eVertices
	};

	struct MeshData
	{
	private:

		using RawData = Vector<std::byte>;

		static std::byte const* ToBytes(void const* anything)
		{
			return reinterpret_cast<std::byte const*>(anything);
		}

	public:
		MeshData() = default;

		MeshData(void const* vtxData, size_t vtxSize, size_t numVerts, void const* idxData = nullptr, size_t idxSize = sizeof(uint32_t), size_t numIndices = 0) :
			Vertices(ToBytes(vtxData), ToBytes(vtxData) + (vtxSize * numVerts)),
			Indices(ToBytes(idxData), ToBytes(idxData) + (idxSize * numIndices)),
			ElementInformation(Indices.Empty() ? (MeshElementType)(numVerts) :
				(idxSize == sizeof(uint32_t) ? MeshElementType::eUint32 : MeshElementType::eUint16))
		{
			MOE_ASSERT(numVerts < moe::MaxValue<uint32_t>() && numIndices < moe::MaxValue<uint32_t>());
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
		MeshElementType	ElementInformation{ MeshElementType::eUint32 };
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
