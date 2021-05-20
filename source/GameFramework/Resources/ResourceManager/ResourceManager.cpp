#include "ResourceManager.h"

namespace moe
{
	MeshResource ResourceManager::LoadMesh(std::string_view meshID, size_t vertexSize, size_t numVertices, const void* vertexData,
	                                       size_t numIndices, const void* indexData, vk::IndexType indexType)
	{
		auto createRscFun = [&]()
		{
			return m_meshFactory->CreateMesh(vertexSize, numVertices, vertexData, numIndices, indexData, indexType);
		};

		return FindOrCreateResource<MeshResource>(meshID.data(), m_meshFactory, createRscFun);
	}

	TextureResource ResourceManager::LoadTextureFile(std::string_view filename, VulkanTextureBuilder& builder)
	{
		auto createRscFun = [&]()
		{
			return m_textureFactory->CreateTextureFromFile(filename, builder);
		};

		return FindOrCreateResource<TextureResource>(filename.data(), m_textureFactory, createRscFun);
	}
}
