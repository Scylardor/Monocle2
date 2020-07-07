// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Containers/Vector/Vector.h"

#include "Graphics/Mesh/Mesh.h"

#include "Graphics/Shader/Handle/ShaderHandle.h"

#include "Graphics/Material/MaterialDescriptor.h"

#include "Graphics/Sampler/SamplerDescriptor.h"

struct aiNode;
struct aiMesh;
struct aiScene;


namespace moe
{
	class MaterialLibrary;

	struct ModelDescriptor
	{
		std::string	m_modelFilename{""};
		ShaderProgramHandle	m_shaderProgram;
	};


	/* A model is an aggregate of meshes.
	 * It also holds, for each mesh, the vertex layout and a material they can be used with.
	 * The material
	 */
	class Model
	{

	public:
		using MeshStorage = Vector<Mesh*>;

		Model(RenderWorld& renderWorld, MaterialLibrary& matLib, const ModelDescriptor& modelDesc);


		MeshStorage::Iterator	begin()
		{
			return m_meshes.Begin();
		}

		MeshStorage::ConstIterator	begin() const
		{
			return m_meshes.End();
		}

		MeshStorage::Iterator	end()
		{
			return m_meshes.End();
		}

		MeshStorage::ConstIterator	end() const
		{
			return m_meshes.End();
		}

	private:

		using TextureCache = HashMap<std::string, Texture2DHandle>;

		void	ProcessNode(RenderWorld& renderWorld, MaterialLibrary& matLib, const std::string& modelDir, TextureCache& textureCache, ShaderProgramHandle shaderHandle, aiNode* node, const aiScene* scene);
		Mesh*	ProcessMesh(RenderWorld& renderWorld, MaterialLibrary& matLib, const std::string& modelDir, TextureCache& textureCache, ShaderProgramHandle shaderHandle, const aiMesh* mesh, const aiScene* scene);

		MeshStorage	m_meshes;
	};

}
