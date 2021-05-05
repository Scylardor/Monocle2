#pragma once

#include "Core/Containers/AssetRegistry/ObjectRegistry.h"
#include "GameFramework/Resources/Factories/AbstractResourceFactory.h"
#include "Graphics/Vulkan/Mesh/VulkanMesh.h"

#include "GameFramework/Resources/Resource/Resource.h"

#include <assimp/importer.hpp>
#include <assimp/postprocess.h>

namespace moe
{
	class AssimpImporter
	{
	public:

		AssimpImporter(ObjectRegistry<VulkanMesh>& registry) :
			IResourceFactory(),
			m_registry(&registry)
		{}

		inline IResource CreateResource() override;


	private:

		ObjectRegistry<VulkanMesh>*	m_registry{};
		Assimp::Importer			m_importer{};


	};


	IResource AssimpMeshFactory::CreateResource()
	{
		static_assert(sizeof(IResource) == sizeof(Resource<VulkanMesh>), "Type erasure is not gonna work (resource size mismatch)");
		return IResource(*m_registry, 9);
	}
}
