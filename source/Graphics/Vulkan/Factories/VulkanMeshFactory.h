#pragma once

#include "Core/Resource/ResourceFactory.h"
#include "Graphics/Vulkan/Mesh/VulkanMesh.h"

namespace moe
{
	class IMeshFactory : public IResourceFactory
	{
	public:

		virtual RegistryID	CreateMesh(size_t vertexSize, size_t numVertices, const void* vertexData,
			size_t numIndices, const void* indexData, vk::IndexType indexType) = 0;


		[[nodiscard]] virtual VulkanMesh&		MutateResource(RegistryID id) = 0;
		[[nodiscard]] virtual const VulkanMesh&	GetResource(RegistryID id) const = 0;
	};


	class VulkanMeshFactory : public IMeshFactory
	{
	public:
		VulkanMeshFactory() = default;

		VulkanMeshFactory(MyVkDevice& device) :
			m_device(&device)
		{}

		RegistryID	IncrementReference(RegistryID id) override;
		void		DecrementReference(RegistryID id) override;


		RegistryID	CreateMesh(size_t vertexSize, size_t numVertices, const void* vertexData,
			size_t numIndices, const void* indexData, vk::IndexType indexType) override;


		[[nodiscard]] VulkanMesh& MutateResource(RegistryID id) override
		{
			return m_meshes.MutEntry(id);
		}


		[[nodiscard]] const VulkanMesh& GetResource(RegistryID id) const override
		{
			return m_meshes.GetEntry(id);
		}


	private:

		MyVkDevice* m_device{ nullptr };

		ObjectRegistry<VulkanMesh>	m_meshes;
	};

}