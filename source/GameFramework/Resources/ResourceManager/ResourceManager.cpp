#include "ResourceManager.h"

#ifdef MOE_VULKAN
	moe::Resource<moe::VulkanMesh> moe::VulkanMeshFactory::CreateMesh(size_t vertexSize, size_t numVertices,
		const void* vertexData, size_t numIndices, const void* indexData, vk::IndexType indexType)
	{
		auto ID = m_registry.EmplaceEntry(m_device, vertexSize, numVertices, vertexData, numIndices, indexData, indexType);
		return { m_registry, ID };
	}
#endif