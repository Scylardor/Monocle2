#pragma once

#include "ResourceRef.h"
#include "Core/Containers/Vector/Vector.h"
#include "Core/Resource/BaseResource.h"

namespace moe
{
	class MaterialResource;
	class IMeshFactory;


	class TextureResource : public IBaseResource
	{
	public:

		TextureResource() = default;

		~TextureResource() override = default;

	};


	class ShaderResource : public IBaseResource
	{
	public:

		ShaderResource() = default;

		~ShaderResource() override = default;

	};


	class ShaderProgramResource : public IBaseResource
	{
	public:

		ShaderProgramResource() = default;

		~ShaderProgramResource() override = default;

	};


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

		std::byte const*	ToBytes(void const* anything)
		{
			return reinterpret_cast<std::byte const*>(anything);
		}

	public:
		MeshData() = default;

		MeshData(void const*  vtxData, size_t vtxSize, size_t numVerts, void const* idxData, size_t idxSize, size_t numIndices) :
			Vertices(ToBytes(vtxData), ToBytes(vtxData) + (vtxSize * numVerts)),
			Indices(ToBytes(idxData), ToBytes(idxData) + (idxSize * numIndices)),
			MeshIndexType((idxSize == sizeof(uint32_t) ? MeshIndexType::eUint32 : MeshIndexType::eUint16))
		{
			MOE_ASSERT(idxSize == sizeof(uint32_t) || idxSize == sizeof(uint16_t));
		}

		RawData			Vertices{};
		RawData			Indices{};
		MeshIndexType	MeshIndexType{ MeshIndexType::eUint32 };
	};


	class MeshResource : public IBaseResource
	{
	public:

		MeshResource() = default;

		MeshResource(void const* vtxData, size_t vtxSize, size_t numVerts, void const* idxData, size_t idxSize, size_t numIndices) :
			Data(vtxData, vtxSize, numVerts, idxData, idxSize, numIndices)
		{}

		~MeshResource() override = default;

		MeshData	Data;
	};


	class AMaterialModule
	{
	public:

		AMaterialModule(uint8_t setNumber) :
			m_setNumber(setNumber)
		{}


		virtual ~AMaterialModule() = default;


		virtual void	UpdateResources(MaterialResource& updatedMaterial) = 0;


	protected:

		uint8_t	m_setNumber{ 0xFF };

	};

	class MaterialResource : public IBaseResource
	{
	public:

		MaterialResource() = default;

		~MaterialResource() override = default;

		virtual std::unique_ptr<MaterialResource> Clone() = 0;

		template <typename TModule, typename... Args>
		TModule&	EmplaceModule(uint8_t setNumber, Args&&... args)
		{
			static_assert(std::is_base_of_v<AMaterialModule, TModule>);
			auto modulePtr = std::make_unique<TModule>(setNumber, std::forward<Args>(args)...);
			// get the ptr before it gets moved
			TModule* ptr = modulePtr.get();

			AddNewModule(std::move(modulePtr));

			return *ptr;
		}


		virtual void				UpdateResources(uint8_t resourceSet) = 0;

		virtual MaterialResource&	BindTextureResource(uint32_t set, uint32_t binding, const Ref<TextureResource>& tex) = 0;

	protected:

		virtual void	AddNewModule(std::unique_ptr<AMaterialModule> newModule) = 0;

	};


	class ShaderPipelineResource : public IBaseResource
	{
	public:

		ShaderPipelineResource() = default;

		~ShaderPipelineResource() override = default;

	};


	class IAssetImporter
	{

	public:

		enum class NormalsGeneration
		{
			Enabled,
			SmoothEnabled,
			Disabled
		};

		bool				Triangulate{ true };
		bool				FlipUVs{ false };
		NormalsGeneration	GenerateNormals{ NormalsGeneration::Disabled };
	};

}
