#pragma once

#include "GameFramework/Resources/Resource/Resource.h"



#include "Graphics/Vertex/VertexFormats.h"
#include "Graphics/Vulkan/MaterialLibrary/VulkanMaterial.h"
#include "Graphics/Vulkan/Mesh/VulkanMesh.h"


#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

struct aiMesh;
struct aiNode;

namespace moe
{

	struct MeshData
	{
		MeshData() = default;

		MeshData(std::vector<BasicVertex>&& verts, std::vector<uint32_t>&& indices) :
			Vertices(std::move(verts)), Indices(std::move(indices))
		{}

		std::vector<BasicVertex>	Vertices;
		std::vector<uint32_t>		Indices;
	};

	struct ModelMesh
	{
		MeshResource		Mesh;
		MaterialResource	Material;
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


		const std::vector<MeshData>&	GetMeshes() const
		{
			return m_meshes;
		}


	private:
		std::vector<ModelNode>	m_nodes;
		std::vector<MeshData>	m_meshes;

	};


	class AssetImporter
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
		NormalsGeneration	GenerateNormals{NormalsGeneration::Disabled};
	};


	class AssimpImporter : public AssetImporter
	{
	public:

		AssimpImporter(class ResourceManager& manager) :
			m_manager(manager)
		{}


		void	ImportModel(std::string_view modelFilename);


	private:

		void	ImportModelResources(Model& importedModel);


		static void						ProcessSceneNode(aiNode& node, const aiScene& scene, Model& importedModel, uint32_t parentIndex = ModelNode::ROOT_INDEX);

		static std::vector<BasicVertex>	ComputeMeshVertices(const aiMesh& mesh);
		static std::vector<uint32_t>	ComputeMeshIndices(const aiMesh& mesh);


		aiPostProcessSteps	ComputeAssimpPostProcessFlags() const;

		ResourceManager& m_manager;

		Assimp::Importer	m_importer{};

	};
}
