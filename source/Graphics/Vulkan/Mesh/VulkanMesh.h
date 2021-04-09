#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/Buffer/VulkanBuffer.h"

namespace moe
{
	class VulkanMesh
	{
	public:

		VulkanMesh() = default;
		VulkanMesh(MyVkDevice& device, size_t vertexSize, size_t numVertices, const void* vertexData,
			size_t numIndices, const void* indexData, vk::IndexType indexType);

		void	Draw(vk::CommandBuffer drawCmdBuffer) const;

	protected:

	private:

		VulkanBuffer	m_vertexBuffer{};
		uint32_t		m_nbVertices{ 0 };

		VulkanBuffer	m_indexBuffer{};
		vk::IndexType	m_indexType{ vk::IndexType::eUint32 };
		uint32_t		m_nbIndices{ 0 };

	};
}


#endif // MOE_VULKAN
