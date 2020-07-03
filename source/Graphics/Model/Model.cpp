// Monocle Game Engine source files - Alexandre Baron

#include "Model.h"

#include "Graphics/RenderWorld/RenderWorld.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace moe
{
	struct VertexPositionNormalTexture
	{
		Vec3	m_position;
		Vec3	m_normal;
		Vec2	m_texcoords;
	};

	void	Model::ProcessNode(RenderWorld& renderWorld, const std::string& modelDir, aiNode* node, const aiScene* scene)
	{
		// process all the node's meshes (if any)
		m_meshes.Reserve(m_meshes.Size() + node->mNumMeshes);
		for (unsigned int iMesh = 0; iMesh < node->mNumMeshes; iMesh++)
		{
			const aiMesh* mesh = scene->mMeshes[node->mMeshes[iMesh]];
			m_meshes.PushBack(ProcessMesh(renderWorld, modelDir, mesh, scene));
		}

		// then do the same for each of its children
		for (unsigned int iChild = 0; iChild < node->mNumChildren; iChild++)
		{
			ProcessNode(renderWorld, modelDir, node->mChildren[iChild], scene);
		}
	}


	Mesh* Model::ProcessMesh(RenderWorld& renderWorld, const std::string& modelDir, const aiMesh* mesh, const aiScene* scene)
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

		// process material
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			for (unsigned int iTex = 0; iTex < material->GetTextureCount(aiTextureType_DIFFUSE); iTex++)
			{
				aiString str;
				material->GetTexture(aiTextureType_DIFFUSE, iTex , &str);

				Texture2DFileDescriptor texFileDesc;
				StringFormat(texFileDesc.m_filename, "%s/%s", modelDir, str.C_Str());

				Texture2DHandle texImg = renderWorld.MutRenderer().MutGraphicsDevice().CreateTexture2D(texFileDesc);
			}

			//vector<Texture> diffuseMaps = loadMaterialTextures(material,
			//	aiTextureType_DIFFUSE, "texture_diffuse");
			//textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			//vector<Texture> specularMaps = loadMaterialTextures(material,
			//	aiTextureType_SPECULAR, "texture_specular");
			//textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		Mesh * newMesh = renderWorld.CreateStaticMeshFromBuffer(vtxData, idxData);
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


	Model::Model(RenderWorld& renderWorld, const ModelDescriptor& modelDesc)
	{
		Assimp::Importer importer;

		// aiProcess_Triangulate means that if the model does not (entirely) consist of triangles,
		// it should transform all the model's primitive shapes to triangles first.
		// Don't flip UVs: stb_image already does
		const aiScene* scene = importer.ReadFile(modelDesc.m_modelFilename,
		aiProcess_Triangulate);

		/* Extract the directory from the model file path. It will help us to retrieve textures, assuming they are stored next to the model. */
		// TODO: use C++17 filesystem parent_path() instead.
		const std::string modelDirectory = modelDesc.m_modelFilename.substr(0, modelDesc.m_modelFilename.find_last_of('/'));

		if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
		{
			MOE_ERROR(ChanGraphics, "Assimp could not import file %s : '%s'.",
			modelDesc.m_modelFilename, importer.GetErrorString());
			return;
		}


		ProcessNode(renderWorld, modelDirectory, scene->mRootNode, scene);

	}


}
