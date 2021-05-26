#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/Buffer/VulkanBuffer.h"
#include "Core/Resource/Resource.h"

namespace moe
{
	class VulkanMesh : public MeshResource
	{
	public:

		VulkanMesh() = default;

		~VulkanMesh();

		explicit VulkanMesh(MyVkDevice& device, size_t vertexSize, size_t numVertices, const void* vertexData,
			size_t numIndices, const void* indexData, vk::IndexType indexType);

		void	Draw(vk::CommandBuffer drawCmdBuffer) const;

		void	Free(MyVkDevice& device);

	protected:

	private:

		MOE_VK_DEVICE_GETTER()


		MyVkDevice*		m_device{ nullptr };
		VulkanBuffer	m_vertexBuffer{};
		uint32_t		m_nbVertices{ 0 };

		VulkanBuffer	m_indexBuffer{};
		vk::IndexType	m_indexType{ vk::IndexType::eUint32 };
		uint32_t		m_nbIndices{ 0 };

	};
}


#endif // MOE_VULKAN
