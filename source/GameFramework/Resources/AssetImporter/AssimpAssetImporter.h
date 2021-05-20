#pragma once

#include "Core/Resource/Resource.h"

#include "Graphics/Vertex/VertexFormats.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

#include <filesystem>

struct aiMaterial;
struct aiMesh;
struct aiNode;

namespace moe
{
	class ResourceManager;

	struct MeshData
	{
		MeshData() = default;

		MeshData(std::vector<BasicVertex>&& verts, std::vector<uint32_t>&& indices) :
			Vertices(std::move(verts)), Indices(std::move(indices))
		{}

		[[nodiscard]] bool	HasVertices() const
		{
			return !Vertices.empty();
		}

		std::vector<BasicVertex>	Vertices;
		std::vector<uint32_t>		Indices;
		std::string					Name{};
	};

	struct ModelMesh
	{
		MeshResource		Mesh;
		MaterialResource	Material;
	};


	struct ModelMaterial
	{

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

		Model(uint32_t expectedMeshes = 0) :
			m_meshes(expectedMeshes)
		{
		}

		ModelNode&	NewNode(uint32_t parentIdx, uint32_t expectedChildrenCount, uint32_t numMeshes)
		{
			auto& node = m_nodes.emplace_back(parentIdx, expectedChildrenCount);

			if (parentIdx != ModelNode::ROOT_INDEX)
				m_nodes[parentIdx].Children.emplace_back(m_nodes.size()-1);

			node.Meshes.reserve(numMeshes);
			return node;
		}

		void	ReserveNodes(uint32_t nbReserved)
		{
			m_nodes.reserve(nbReserved);
		}

		void	ImportMeshResources(ResourceManager& manager);


		MeshData&	MutMesh(uint32_t meshIdx)
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


		[[nodiscard]] const std::vector<MeshData>&	GetMeshes() const
		{
			return m_meshes;
		}


		[[nodiscard]] const std::vector<MeshResource>& GetMeshResources() const
		{
			return meshResources;
		}


	private:
		std::vector<ModelNode>		m_nodes;
		std::vector<MeshData>		m_meshes;
		std::vector<MeshResource>	meshResources;
	};



	class AssimpImporter : public IAssetImporter
	{
		using FilePath = std::filesystem::path;

	public:

		AssimpImporter(class ResourceManager& manager) :
			m_manager(manager)
		{}


		Model	ImportModel(std::string_view modelFilename);


	private:

		void	ImportModelResources(Model& importedModel);

		void	ExtractMaterialTextures(const FilePath& basePath, const aiMaterial& material);


		void	ProcessSceneNode(aiNode& node, const aiScene& scene, Model& importedModel, const FilePath& modelPath, uint32_t parentIndex = ModelNode::ROOT_INDEX);


		[[nodiscard]] static std::vector<BasicVertex>	ComputeMeshVertices(const aiMesh& mesh);
		[[nodiscard]] static std::vector<uint32_t>		ComputeMeshIndices(const aiMesh& mesh);

		[[nodiscard]] aiPostProcessSteps				ComputeAssimpPostProcessFlags() const;

		ResourceManager&	m_manager;

		Assimp::Importer	m_importer{};

	};
}
