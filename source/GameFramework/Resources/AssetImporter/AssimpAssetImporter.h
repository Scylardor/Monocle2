#pragma once

#ifdef MOE_VULKAN
#include "Graphics/Vertex/VertexFormats.h"

#include "Core/Resource/ResourceRef.h"

#include "Core/Resource/Resource.h"
#include "Graphics/Vulkan/Material/VulkanMaterial.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include <assimp/material.h>

#include <filesystem>

struct aiMaterial;
struct aiMesh;
struct aiNode;

namespace moe
{
	class ResourceManager;
	class MyVkDevice;

	struct ModelData
	{
		ModelData() = default;

		ModelData(std::vector<Vertex_PosColorUV>&& verts, std::vector<uint32_t>&& indices) :
			Vertices(std::move(verts)), Indices(std::move(indices))
		{}

		[[nodiscard]] bool	HasVertices() const
		{
			return !Vertices.empty();
		}

		std::vector<Vertex_PosColorUV>	Vertices;
		std::vector<uint32_t>			Indices;
		std::string						Name{};
		uint32_t						Material{ 0 };
	};

	struct ModelMesh
	{
		Ref<MeshResource>		Mesh;
		Ref<MaterialResource>	Material;
	};



	struct ModelMaterial
	{
		std::string													Name{};
		std::vector<std::pair<aiTextureType, Ref<TextureResource>>>	Textures{};
		PhongReflectivityParameters									ReflectivityParams{};
		Ref<MaterialResource>										MaterialResource{};
	};


	struct ModelNode
	{
		static const auto ROOT_INDEX = ~0u;

		ModelNode(uint32_t parentIdx, uint32_t expectedChildren) :
			Parent(parentIdx)
		{
			Children.reserve(expectedChildren);
		}

		void	EmplaceMesh(size_t meshIdx)
		{
			Meshes.emplace_back(meshIdx);
		}

		void	AddChild(uint32_t child)
		{
			Children.push_back(child);
		}


		uint32_t					Parent{ ROOT_INDEX };
		std::vector<size_t>			Children{};
		std::vector<size_t>			Meshes{};
	};


	// A model is basically a simple tree-node structure
	// modeling the parent-children relationship between multiple meshes and their materials.
	class Model
	{
	public:

		Model(std::string_view name = "", uint32_t expectedMeshes = 0, uint32_t expectedMaterials = 0) :
			m_name(name),
			m_meshes(expectedMeshes),
			m_materials(expectedMaterials)
		{
		}

		ModelNode& NewNode(uint32_t parentIdx, uint32_t expectedChildrenCount, uint32_t numMeshes);

		void	ReserveNodes(uint32_t nbReserved)
		{
			m_nodes.reserve(nbReserved);
		}

		void	ImportMeshResources(ResourceManager& manager, MyVkDevice& gfxDevice);


		ModelData&	MutMesh(uint32_t meshIdx)
		{
			return m_meshes[meshIdx];
		}


		[[nodiscard]] size_t	NodeCount() const
		{
			return m_nodes.size();
		}


		[[nodiscard]] size_t	MeshesCount() const
		{
			return m_meshes.size();
		}

		[[nodiscard]] size_t	NumMaterials() const
		{
			return m_materials.size();
		}

		[[nodiscard]] ModelMaterial&	MutMaterial(uint32_t matIndex)
		{
			MOE_ASSERT(matIndex < m_materials.size());
			return m_materials[matIndex];
		}


		[[nodiscard]] const std::vector<ModelData>&	GetMeshes() const
		{
			return m_meshes;
		}


		[[nodiscard]] const std::vector<Ref<MeshResource>>& GetMeshResources() const
		{
			return m_meshResources;
		}


		[[nodiscard]] const std::vector<ModelMaterial>& GetMaterialResources() const
		{
			return m_materials;
		}


		[[nodiscard]] const std::vector<ModelNode>& GetNodes() const
		{
			return m_nodes;
		}


		[[nodiscard]] const std::string&	GetName() const
		{
			return m_name;
		}


	private:
		std::string						m_name{};
		std::vector<ModelNode>			m_nodes;
		std::vector<ModelData>			m_meshes;
		std::vector<Ref<MeshResource>>	m_meshResources;
		std::vector<ModelMaterial>		m_materials;
	};



	class AssimpImporter : public IAssetImporter
	{
		using FilePath = std::filesystem::path;

	public:

		AssimpImporter(class ResourceManager& manager, MyVkDevice& device) :
			m_manager(manager),
			m_gfxDevice(device)
		{}


		Model	ImportModel(std::string_view modelFilename);


	private:

		void	ImportSceneResources(const aiScene& scene, Model& importedModel, const FilePath& modelPath);

		void	ImportSceneMaterial(const aiScene& scene, uint32_t materialIndex, Model& importedModel, const FilePath& basePath, Ref<MaterialResource>& defaultMaterial);


		void	ProcessSceneNode(aiNode& node, const aiScene& scene, Model& importedModel, uint32_t parentIndex = ModelNode::ROOT_INDEX);


		[[nodiscard]] static std::vector<Vertex_PosColorUV>	ComputeMeshVertices(const aiMesh& mesh);
		[[nodiscard]] static std::vector<uint32_t>			ComputeMeshIndices(const aiMesh& mesh);

		[[nodiscard]] aiPostProcessSteps				ComputeAssimpPostProcessFlags() const;

		ResourceManager&	m_manager;
		MyVkDevice&			m_gfxDevice;

		Assimp::Importer	m_importer{};

	};
}

#endif