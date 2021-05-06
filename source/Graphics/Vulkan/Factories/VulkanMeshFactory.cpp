
#include "VulkanMeshFactory.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"


namespace moe
{
	RegistryID VulkanMeshFactory::IncrementReference(RegistryID id)
	{
		return m_meshes.IncrementReference(id);
	}


	void VulkanMeshFactory::DecrementReference(RegistryID id)
	{
		if (m_meshes.GetReferenceCount(id) == 1 && false == m_meshes.IsPersistent(id))
		{
			// Last ref : free the mesh
			auto& vulkanMesh = m_meshes.MutEntry(id);
			vulkanMesh.Free(*m_device);
		}

		m_meshes.DecrementReference(id);
	}


	RegistryID VulkanMeshFactory::CreateMesh(size_t vertexSize, size_t numVertices, const void* vertexData,
	                                         size_t numIndices, const void* indexData, vk::IndexType indexType)
	{
		auto newMeshID = m_meshes.EmplaceEntry(*m_device, vertexSize, numVertices, vertexData, numIndices, indexData, indexType);
		return newMeshID;
	}
}
