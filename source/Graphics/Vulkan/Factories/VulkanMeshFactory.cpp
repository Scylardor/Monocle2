#ifdef MOE_VULKAN
#include "VulkanMeshFactory.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"


namespace moe
{
	RegistryID VulkanMeshFactory::IncrementReference(RegistryID id)
	{
		return m_meshes.IncrementReference(id);
	}


	bool VulkanMeshFactory::DecrementReference(RegistryID id)
	{
		const bool toBeFreed = m_meshes.GetReferenceCount(id) == 1 && false == m_meshes.IsPersistent(id);

		if (m_meshes.GetReferenceCount(id) == 1 && false == m_meshes.IsPersistent(id))
		{
			// Last ref : free the mesh
			auto& vulkanMesh = m_meshes.MutEntry(id);
			vulkanMesh.Free(*m_device);
		}

		m_meshes.DecrementReference(id);

		return toBeFreed;
	}


	RegistryID VulkanMeshFactory::CreateMesh(size_t vertexSize, size_t numVertices, const void* vertexData,
	                                         size_t numIndices, const void* indexData, MeshIndexType indexType)
	{
		auto newMeshID = m_meshes.EmplaceEntry(*m_device, vertexSize, numVertices, vertexData, numIndices, indexData, (vk::IndexType) indexType);
		return newMeshID;
	}


	std::unique_ptr<MeshResource> VulkanMeshFactory::NewMesh(size_t vertexSize, size_t numVertices,
		const void* vertexData, size_t numIndices, const void* indexData, MeshIndexType indexType)
	{
		return std::make_unique<VulkanMesh>(*m_device, vertexSize, numVertices, vertexData, numIndices, indexData, (vk::IndexType) indexType);
	}
}
#endif