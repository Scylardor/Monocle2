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
	struct ModelDescriptor
	{
		std::string	m_modelFilename{""};
		ShaderProgramHandle	m_shaderProgram;
		Vector<MaterialDescriptor>	m_meshMaterialDescs;
	};


	/* A model is an aggregate of meshes.
	 * It also holds, for each mesh, the vertex layout and a material they can be used with.
	 * The material
	 */
	class Model
	{

	public:
		using MeshStorage = Vector<Mesh*>;

		Model(RenderWorld& renderWorld, const ModelDescriptor& modelDesc);


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

		void	ProcessNode(RenderWorld& renderWorld, const std::string& modelDir, aiNode* node, const aiScene* scene);
		Mesh*	ProcessMesh(RenderWorld& renderWorld, const std::string& modelDir, const aiMesh* mesh, const aiScene* scene);

		MeshStorage	m_meshes;
	};

}
