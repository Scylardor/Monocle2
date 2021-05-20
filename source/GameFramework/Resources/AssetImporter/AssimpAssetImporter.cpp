#include "AssimpAssetImporter.h"

#include "Graphics/Vertex/VertexFormats.h"

#include "GameFramework/Resources/ResourceManager/ResourceManager.h"

#include <assimp/scene.h>

#include "Graphics/Vulkan/Texture/VulkanTexture.h"


void moe::Model::ImportMeshResources(ResourceManager& manager)
{
	meshResources.reserve(MeshesCount());

	for (const auto& mesh : GetMeshes())
	{
		meshResources.emplace_back(
			manager.LoadMesh(
				mesh.Name,
				sizeof(mesh.Vertices[0]), mesh.Vertices.size(), mesh.Vertices.data(),
				mesh.Indices.size(), mesh.Indices.data(), vk::IndexType::eUint32)
		);

	}
}


moe::Model moe::AssimpImporter::ImportModel(std::string_view modelFilename)
{
	aiPostProcessSteps ppFlags = ComputeAssimpPostProcessFlags();
	const aiScene* scene = m_importer.ReadFile(modelFilename.data(), ppFlags);
	if (scene == nullptr)
	{
		MOE_ERROR(moe::ChanDefault, "Could not import model file %s (%s).", modelFilename.data(), m_importer.GetErrorString());
		return {};
	}

	if (scene->mRootNode == nullptr)
	{
		MOE_WARNING(moe::ChanDefault, "Tried to import an empty model file (%s).", modelFilename.data());
		return {};
	}

	// The model name is going to be the filename stripped of the extension
	FilePath modelPath = FilePath(modelFilename).remove_filename().make_preferred();

	// Expect the given number of meshes
	Model importedModel{scene->mNumMeshes};

	ProcessSceneNode(*scene->mRootNode, *scene, importedModel, modelPath);

	ImportModelResources(importedModel);

	return importedModel;
}


void moe::AssimpImporter::ImportModelResources(Model& importedModel)
{
	// First import the meshes
	importedModel.ImportMeshResources(m_manager);

	// Then the materials


}

void moe::AssimpImporter::ProcessSceneNode(aiNode& node, const aiScene& scene, Model& importedModel, const FilePath& modelPath, uint32_t parentIndex)
{
	importedModel.ReserveNodes((uint32_t) importedModel.NodeCount() + node.mNumChildren); // reserve memory for this node + children

	ModelNode& myNode = importedModel.NewNode(parentIndex, node.mNumChildren, node.mNumMeshes);
	const auto childrenParentIndex = (uint32_t) importedModel.NodeCount() - 1;

	for (auto iMesh = 0u; iMesh < node.mNumMeshes; ++iMesh)
	{
		aiMesh* mesh = scene.mMeshes[node.mMeshes[iMesh]];
		MOE_ASSERT(mesh);
		aiMesh& meshRef = *mesh;

		MeshData& thisMesh = importedModel.MutMesh(node.mMeshes[iMesh]);
		// Skip computing mesh data if we have the vertices (we already processed it)
		if (thisMesh.HasVertices())
			continue;

		thisMesh.Vertices = ComputeMeshVertices(meshRef);

		thisMesh.Indices = ComputeMeshIndices(meshRef);

		thisMesh.Name = meshRef.mName.C_Str();

		myNode.Meshes.assign(node.mMeshes, node.mMeshes + node.mNumMeshes);

		MOE_ASSERT(scene.HasMaterials() && meshRef.mMaterialIndex < scene.mNumMaterials);
		aiMaterial* meshMat = scene.mMaterials[meshRef.mMaterialIndex];
		MOE_ASSERT(meshMat);
		ExtractMaterialTextures(modelPath, *meshMat);
	}

	// Do the same for all the children
	for (auto iChild = 0u; iChild < node.mNumChildren; ++iChild)
	{
		ProcessSceneNode(*node.mChildren[iChild], scene, importedModel, modelPath, childrenParentIndex);
	}
}


void moe::AssimpImporter::ExtractMaterialTextures(const FilePath& basePath, const aiMaterial& material)
{
	// Technical limitation of the importer : we expect only one texture per type.
	// We also expect the texture filename to be relative to the base path.
	// For an OBJ for example, it means we expect the .obj and .mtl files to be together.
	static const auto SUPPORTED_TEXTURES_TYPES = {
		aiTextureType_AMBIENT,
		aiTextureType_DIFFUSE,
		aiTextureType_SPECULAR,
		aiTextureType_NORMALS,
		aiTextureType_HEIGHT,
		aiTextureType_EMISSIVE,
		aiTextureType_SHININESS,
		aiTextureType_LIGHTMAP
	};

	VulkanTextureBuilder builder;

	for (auto texType : SUPPORTED_TEXTURES_TYPES)
	{
		if (material.GetTextureCount(texType) > 0)
		{
			aiString texPath{};
			material.GetTexture(texType, 0, &texPath);
			const FilePath textureFilePath = basePath / texPath.C_Str();

			m_manager.LoadTextureFile(textureFilePath.string(), builder);
		}
	}
}


std::vector<moe::BasicVertex> moe::AssimpImporter::ComputeMeshVertices(const aiMesh& mesh)
{
	std::vector<BasicVertex> vertexData;
	vertexData.resize(mesh.mNumVertices);

	const bool hasColors = mesh.HasVertexColors(0);
	const bool hasUV0 = mesh.HasTextureCoords(0);

	for (auto iVtx = 0u; iVtx < mesh.mNumVertices; ++iVtx)
	{
		const auto& pos = mesh.mVertices[iVtx];
		vertexData[iVtx].Position = { pos.x, pos.y, pos.z };

		if (hasColors)
		{
			const auto& color = mesh.mColors[0][iVtx];
			vertexData[iVtx].Color = { color.r, color.g, color.b };
		}

		if (hasUV0)
		{
			const auto& uv0 = mesh.mTextureCoords[0][iVtx];
			vertexData[iVtx].Texture_UV0 = { uv0.x, uv0.y };
		}
	}

	return vertexData;
}


std::vector<uint32_t> moe::AssimpImporter::ComputeMeshIndices(const aiMesh& mesh)
{
	std::vector<uint32_t> indices;

	// Expect triangles, so 3 indices per size
	indices.reserve(mesh.mNumFaces * 3);

	for (auto iFace = 0u; iFace < mesh.mNumFaces; ++iFace)
	{
		const auto& face = mesh.mFaces[iFace];

		for (auto iInd = 0u; iInd < face.mNumIndices; ++iInd)
		{
			indices.push_back(face.mIndices[iInd]);
		}
	}

	return indices;
}


aiPostProcessSteps moe::AssimpImporter::ComputeAssimpPostProcessFlags() const
{
	// aiProcess_Triangulate tells Assimp that if the model does not (entirely) consist of triangles, it should transform all the model's primitive shapes to triangles first.
	unsigned ppFlags{};
	if (Triangulate)
		ppFlags |= aiProcess_Triangulate;

	if (FlipUVs)
		ppFlags |= aiProcess_FlipUVs;

	switch (GenerateNormals)
	{
	case NormalsGeneration::Enabled:
		ppFlags |= aiProcess_GenNormals;
		break;
	case NormalsGeneration::SmoothEnabled:
		ppFlags |= aiProcess_GenSmoothNormals;
		break;
	case NormalsGeneration::Disabled: break;
	}

	return (aiPostProcessSteps)ppFlags;
}
