#ifdef MOE_VULKAN

#include "AssimpAssetImporter.h"

#include "Graphics/Vertex/VertexFormats.h"

#include "Core/Resource/ResourceManager.h"

#include <assimp/scene.h>

#include "Graphics/Vulkan/Device/VulkanDevice.h"
#include "Graphics/Vulkan/Texture/VulkanTexture.h"


moe::ModelNode& moe::Model::NewNode(uint32_t parentIdx, uint32_t expectedChildrenCount, uint32_t numMeshes)
{
	auto& node = m_nodes.emplace_back(parentIdx, expectedChildrenCount);

	if (parentIdx != ModelNode::ROOT_INDEX)
		m_nodes[parentIdx].Children.emplace_back(m_nodes.size() - 1);

	node.Meshes.reserve(numMeshes);
	return node;
}


void moe::Model::ImportMeshResources(ResourceManager& manager, MyVkDevice& gfxDevice)
{
	m_meshResources.reserve(MeshesCount());

	const auto meshLoadFn = [&](const MeshData& mesh)
	{
		return [&]()
		{
			return gfxDevice.MeshFactory.NewMesh(sizeof(mesh.Vertices[0]), mesh.Vertices.size(), mesh.Vertices.data(),
				mesh.Indices.size(), mesh.Indices.data(), VertexIndexType::eUint32);
		};
	};

	for (const auto& mesh : GetMeshes())
	{
		auto meshRef = manager.Load<MeshResource>(HashString(mesh.Name), meshLoadFn(mesh));
		m_meshResources.push_back(std::move(meshRef));
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

	FilePath modelPath(modelFilename);
	// The model name is going to be the filename stripped of the extension
	const std::string modelName = modelPath.filename().replace_extension("").string();

	// Expect the given number of meshes
	Model importedModel{ modelName, scene->mNumMeshes, scene->mNumMaterials};

	// We will need the "base path" to go search for textures (mtl files use relative paths)
	modelPath.remove_filename().make_preferred();

	ProcessSceneNode(*scene->mRootNode, *scene, importedModel);

	ImportSceneResources(*scene, importedModel, modelPath);

	return importedModel;
}


void moe::AssimpImporter::ImportSceneResources(const aiScene& scene, Model& importedModel, const FilePath& modelPath)
{
	// First import the materials, then the meshes
	Ref<MaterialResource> defaultMat = m_manager.FindExisting<MaterialResource>(HashString("DefaultMaterial"));
	MOE_ASSERT(defaultMat);

	// Start material idx at 1 because 0 is the default material and we don't care about it.
	for (auto iMat = 1u; iMat < scene.mNumMaterials; ++iMat)
	{
		ImportSceneMaterial(scene, iMat, importedModel, modelPath, defaultMat);
	}


	importedModel.ImportMeshResources(m_manager, m_gfxDevice);

	// Then the materials

	//m_manager.InsertResource(HashString, std::move(clonedMat));
}

void moe::AssimpImporter::ProcessSceneNode(aiNode& node, const aiScene& scene, Model& importedModel, uint32_t parentIndex)
{
	importedModel.ReserveNodes((uint32_t) importedModel.NodeCount() + node.mNumChildren); // reserve memory for this node + children

	ModelNode& myNode = importedModel.NewNode(parentIndex, node.mNumChildren, node.mNumMeshes);
	const auto childrenParentIndex = (uint32_t) importedModel.NodeCount() - 1;

	myNode.Meshes.assign(node.mMeshes, node.mMeshes + node.mNumMeshes);

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

		thisMesh.Name.resize(importedModel.GetName().size() + meshRef.mName.length + 1);
		moe::StringFormat(thisMesh.Name, "SM_%s_%s", importedModel.GetName(), meshRef.mName.C_Str());

		thisMesh.Material = meshRef.mMaterialIndex;
		MOE_ASSERT(meshRef.mMaterialIndex < scene.mNumMaterials);
	}

	// Do the same for all the children
	for (auto iChild = 0u; iChild < node.mNumChildren; ++iChild)
	{
		ProcessSceneNode(*node.mChildren[iChild], scene, importedModel, childrenParentIndex);
	}
}


void moe::AssimpImporter::ImportSceneMaterial(const aiScene& scene, uint32_t materialIndex, Model& importedModel, const FilePath& basePath, Ref<MaterialResource>& defaultMaterial)
{
	aiMaterial* assimpMat = scene.mMaterials[materialIndex];
	MOE_ASSERT(assimpMat);

	ModelMaterial& modelMat = importedModel.MutMaterial(materialIndex);

	// First import the textures
	{
		// Technical limitation of the importer : we expect only one texture per type.
		// We also expect the texture filename to be relative to the base path.
		// For an OBJ for example, it means we expect the .obj and .mtl files to be together.
		static const auto SUPPORTED_TEXTURES_TYPES = {
			aiTextureType_AMBIENT,		// For ambient / AO
			aiTextureType_DIFFUSE,		// For diffuse / albedo
			aiTextureType_SPECULAR,		// For specular / roughness
			aiTextureType_NORMALS,		// For normal maps (this should be the preferred way)
			aiTextureType_HEIGHT,		// For height / normal maps (depends on the case)
			aiTextureType_EMISSIVE,		// Emissive map
			aiTextureType_SHININESS,	// For specular exponent maps (Blinn-Phong) or metallic (PBR)
			aiTextureType_LIGHTMAP,		// For lightmapping
		};

		VulkanTextureBuilder builder;

		for (auto texType : SUPPORTED_TEXTURES_TYPES)
		{
			if (assimpMat->GetTextureCount(texType) > 0)
			{
				aiString texPath{};
				assimpMat->GetTexture(texType, 0, &texPath);
				const FilePath textureFilePath = basePath / texPath.C_Str();

				std::string texPathStr = textureFilePath.string();
				Ref<TextureResource> texRef = m_manager.Load<TextureResource>(HashString(texPathStr), m_gfxDevice.TextureFactory, texPathStr, builder);
				modelMat.Textures.emplace_back(texType, std::move(texRef));
			}
		}
	}

	// Then the reflectivity parameters
	static const auto SetVec4Parameter = [&](Vec4& matParam, auto... materialKey)
	{
		aiColor4D color{ 0.f };
		aiGetMaterialColor(assimpMat, materialKey..., &color);
		matParam = { color.r, color.g, color.b, color.a };
	};

	SetVec4Parameter(modelMat.ReflectivityParams.Ambient, AI_MATKEY_COLOR_AMBIENT);
	SetVec4Parameter(modelMat.ReflectivityParams.Diffuse, AI_MATKEY_COLOR_DIFFUSE);
	SetVec4Parameter(modelMat.ReflectivityParams.Specular, AI_MATKEY_COLOR_SPECULAR);
	SetVec4Parameter(modelMat.ReflectivityParams.Emissive, AI_MATKEY_COLOR_EMISSIVE);

	static const auto SetFloatParameter = [&](float& matParam, auto... materialKey)
	{
		float param;
		aiGetMaterialFloat(assimpMat, materialKey..., &param);
		matParam = param;
	};
	SetFloatParameter(modelMat.ReflectivityParams.SpecularExponent, AI_MATKEY_SHININESS);
	SetFloatParameter(modelMat.ReflectivityParams.Opacity, AI_MATKEY_OPACITY);

	aiString matName;
	aiGetMaterialString(assimpMat, AI_MATKEY_NAME, &matName);

	modelMat.Name.resize(importedModel.GetName().size() + matName.length + 6); // +6: accounts for additional chars and \0
	moe::StringFormat(modelMat.Name, "M_%s_%s", importedModel.GetName(), matName.C_Str());
	auto newMat = defaultMaterial->Clone();
	modelMat.MaterialResource = m_manager.Insert<MaterialResource>(HashString(modelMat.Name), std::move(newMat));
	MOE_ASSERT(modelMat.MaterialResource);

	auto& phongMaps = modelMat.MaterialResource->EmplaceModule<PhongReflectivityMapsMaterialModule>(0);
	// TODO just sets diffuse for now
	auto diffuseIt = std::find_if(modelMat.Textures.begin(), modelMat.Textures.end(),
		[](auto& texInfo)
		{
			return texInfo.first == aiTextureType_DIFFUSE;
		});
	MOE_ASSERT(diffuseIt != modelMat.Textures.end());
	auto diffuseIdx = diffuseIt - modelMat.Textures.begin();

	phongMaps.Set(PhongMap::Diffuse, modelMat.Textures[diffuseIdx].second);
	modelMat.MaterialResource->UpdateResources(0);
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
#endif