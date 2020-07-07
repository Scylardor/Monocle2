// Monocle Game Engine source files - Alexandre Baron

#include "Model.h"

#include "Graphics/RenderWorld/RenderWorld.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Core/Misc/moeCountof.h"

#include "Graphics/Material/MaterialBindings.h"
#include "Graphics/Material/MaterialInterface.h"
#include "Graphics/Material/MaterialLibrary.h"

namespace moe
{
	// TODO: refactor out of here
	struct VertexPositionNormalTexture
	{
		Vec3	m_position;
		Vec3	m_normal;
		Vec2	m_texcoords;
	};

	struct PhongMaterial
	{
		Vec4	m_ambientColor{ 1.f };
		Vec4	m_diffuseColor{ 1.f };
		Vec4	m_specularColor{ 1.f };
		float	m_shininess{ 32 };
	};


	void	Model::ProcessNode(RenderWorld& renderWorld, MaterialLibrary& matLib, const std::string& modelDir, TextureCache& textureCache, ShaderProgramHandle shaderHandle, aiNode* node, const aiScene* scene)
	{
		// process all the node's meshes (if any)
		m_meshes.Reserve(m_meshes.Size() + node->mNumMeshes);
		for (unsigned int iMesh = 0; iMesh < node->mNumMeshes; iMesh++)
		{
			const aiMesh* mesh = scene->mMeshes[node->mMeshes[iMesh]];
			m_meshes.PushBack(ProcessMesh(renderWorld, matLib, modelDir, textureCache, shaderHandle, mesh, scene));
		}

		// then do the same for each of its children
		for (unsigned int iChild = 0; iChild < node->mNumChildren; iChild++)
		{
			ProcessNode(renderWorld, matLib, modelDir, textureCache, shaderHandle, node->mChildren[iChild], scene);
		}
	}


	Mesh* Model::ProcessMesh(RenderWorld& renderWorld, MaterialLibrary& matLib, const std::string& modelDir, TextureCache& textureCache, ShaderProgramHandle shaderHandle, const aiMesh* mesh, const aiScene* scene)
	{
		// For now, assume a model always has at least position, normal, texture coordinates.
		Vector<VertexPositionNormalTexture> vertices;
		vertices.Resize(mesh->mNumVertices);

		MeshDataDescriptor vtxData{ vertices.Data(),
			vertices.Size() * sizeof(VertexPositionNormalTexture), vertices.Size()};

		// process each vertex position, normal and texture coordinates
		for (unsigned int iVert = 0; iVert < mesh->mNumVertices; iVert++)
		{
			// we define a temporary vec3 for transferring Assimp's data to.
			// This is necessary as Assimp maintains its own data types for vector, matrices, strings etc.
			// and they don't convert that well to glm's data types for example.
			const aiVector3D& vtxPos = mesh->mVertices[iVert];

			const aiVector3D& vtxNorm = mesh->mNormals[iVert];

			// Assimp allows a model to have up to 8 different texture coordinates per vertex.
			// So far we only care about the first set of texture coordinates.
			Vec2 texCoords{0.f, 0.f };
			if (mesh->mTextureCoords[0] != nullptr)
			{
				const aiVector3D& vtxUv = mesh->mTextureCoords[0][iVert];
				texCoords = { vtxUv.x, vtxUv.y };
			}

			vertices[iVert] = {
				Vec3{ vtxPos.x, vtxPos.y, vtxPos.z },
				Vec3{ vtxNorm.x, vtxNorm.y, vtxNorm.z },
				texCoords
			};
		}

		// Then, process vertex indices for indexed draw.
		// Assimp defines a mesh as having an array of faces, where each face represents a single primitive.
		// In our case, due to the aiProcess_Triangulate option are always triangles.
		// A face contains the indices of the vertices we need to draw in what order for its primitive.
		Vector<uint32_t> indices;
		indices.Resize(mesh->mNumFaces * 3);

		MeshDataDescriptor idxData{
			indices.Data(), indices.Size() * sizeof(uint32_t), indices.Size()
		};

		unsigned int index = 0;

		for (unsigned int iFace = 0; iFace < mesh->mNumFaces; iFace++)
		{
			const aiFace& face = mesh->mFaces[iFace];

			for (unsigned int iIdx = 0; iIdx < face.mNumIndices; iIdx++)
			{
				indices[index] = face.mIndices[iIdx];
				index++;
			}
		}

		// create the mesh geometry...
		Mesh * newMesh = renderWorld.CreateStaticMeshFromBuffer(vtxData, idxData);

		// ... then process the material
		// TODO: this should be done in some kind of Resource Manager context to reuse multiple instances of the same texture instead of duplicating it.
		// ANd heavily refactored! supportedTextureTypes does not respect open/closed principle...

		if (mesh->mMaterialIndex >= 0)
		{
			// Create a Monocle Material Descriptor and fill it as necessary.
			MaterialDescriptor matDesc(
				{
					{"Material_Phong", ShaderStage::Fragment},
					{"Material_Sampler", ShaderStage::Fragment}
				}
			);

			// Try to retrieve the types of texture we know.
			// So far, we only support one texture map per type of texture.
			const aiTextureType supportedTextureTypes[] = {
				aiTextureType_DIFFUSE,
				aiTextureType_SPECULAR,
				aiTextureType_EMISSIVE,
				aiTextureType_HEIGHT,
				aiTextureType_NORMALS,
				aiTextureType_SHININESS
			};

			constexpr auto numSupportedTexTypes = Countof(supportedTextureTypes);

			const MaterialTextureBinding monocleTextureTypes[numSupportedTexTypes] = {
				DIFFUSE,
				SPECULAR,
				EMISSION,
				HEIGHT,
				NORMAL,
				GLOSS
			};

			// It's not a code smell, it's a whole code trashcan !! It reeks !!!
			const char* monocleTextureDescriptors[numSupportedTexTypes] = {
				"Material_DiffuseMap",
				"Material_SpecularMap",
				"Material_EmissionMap",
				"Material_HeightMap",
				"Material_NormalMap",
				"Material_GlossMap"
			};

			Texture2DHandle textureHandles[numSupportedTexTypes]{0};

			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			for (int iTexType = 0; iTexType < numSupportedTexTypes; iTexType++)
			{
				aiString str;
				aiReturn texExists = material->GetTexture(supportedTextureTypes[iTexType], 0, &str);

				if (texExists == AI_SUCCESS)
				{
					matDesc.AddBinding(monocleTextureDescriptors[iTexType], ShaderStage::Fragment); // assume all textures are used in fragment shader for now...

					// Load the texture while we're at it.
					Texture2DFileDescriptor texFileDesc;
					StringFormat(texFileDesc.m_filename, "%s/%s", modelDir, str.C_Str());

					auto texIt = textureCache.Find(texFileDesc.m_filename);
					if (texIt == textureCache.End())
					{
						textureHandles[iTexType] = renderWorld.MutRenderer().MutGraphicsDevice().CreateTexture2D(texFileDesc);
						textureCache.Insert({ texFileDesc.m_filename , textureHandles[iTexType] });
					}
					else
					{
						textureHandles[iTexType] = texIt->second;
					}
				}
			}

			// Now create Material Interface and Instance
			MaterialInterface matInterface = matLib.CreateMaterialInterface(shaderHandle, matDesc);
			MaterialInstance matInstance = matLib.CreateMaterialInstance(matInterface);

			for (int iTexType = 0; iTexType < numSupportedTexTypes; iTexType++)
			{
				if (textureHandles[iTexType].IsNotNull())
				{
					matInstance.BindTexture(monocleTextureTypes[iTexType], textureHandles[iTexType]);
				}
			}

			// TODO : refactor
			matInstance.UpdateUniformBlock(MaterialBlockBinding::MATERIAL_PHONG,
				PhongMaterial{ ColorRGBAf::White().ToVec(),
								ColorRGBAf::White().ToVec(),
								ColorRGBAf::White().ToVec(),
								32 });

			SamplerDescriptor mySamplerDesc;
			mySamplerDesc.m_magFilter = SamplerFilter::Linear;
			mySamplerDesc.m_minFilter = SamplerFilter::LinearMipmapLinear;
			mySamplerDesc.m_wrap_S = SamplerWrapping::Repeat;
			mySamplerDesc.m_wrap_T = SamplerWrapping::Repeat;

			SamplerHandle mySampler = renderWorld.MutRenderer().MutGraphicsDevice().CreateSampler(mySamplerDesc);

			matInstance.BindSampler(MaterialSamplerBinding::SAMPLER_0, mySampler);

			matInstance.CreateMaterialResourceSet();

			newMesh->BindMaterial(std::move(matInstance));
		}

		return newMesh;
	}



	//Vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
	//{
	//	vector<Texture> textures;
	//	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	//	{
	//		aiString str;
	//		mat->GetTexture(type, i, &str);
	//		Texture texture;
	//		texture.id = TextureFromFile(str.C_Str(), directory);
	//		texture.type = typeName;
	//		texture.path = str;
	//		textures.push_back(texture);
	//	}
	//	return textures;
	//}


	Model::Model(RenderWorld& renderWorld, MaterialLibrary& matLib, const ModelDescriptor& modelDesc)
	{
		Assimp::Importer importer;

		// aiProcess_Triangulate means that if the model does not (entirely) consist of triangles,
		// it should transform all the model's primitive shapes to triangles first.
		// Don't flip UVs: stb_image already does
		const aiScene* scene = importer.ReadFile(modelDesc.m_modelFilename,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		/* Extract the directory from the model file path. It will help us to retrieve textures, assuming they are stored next to the model. */
		// TODO: use C++17 filesystem parent_path() instead.
		const std::string modelDirectory = modelDesc.m_modelFilename.substr(0, modelDesc.m_modelFilename.find_last_of('/'));

		if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
		{
			MOE_ERROR(ChanGraphics, "Assimp could not import file %s : '%s'.",
			modelDesc.m_modelFilename, importer.GetErrorString());
			return;
		}

		// TODO: this should be implemented in a resource manager.
		HashMap<std::string, Texture2DHandle> textureCache;

		ProcessNode(renderWorld, matLib, modelDirectory, textureCache, modelDesc.m_shaderProgram, scene->mRootNode, scene);
	}


}
