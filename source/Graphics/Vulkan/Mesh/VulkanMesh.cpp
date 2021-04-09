#ifdef MOE_VULKAN

#include "VulkanMesh.h"

namespace moe
{
	VulkanMesh::VulkanMesh(MyVkDevice& device, size_t vertexSize, size_t numVertices, const void* vertexData,
		size_t numIndices, const void* indexData, vk::IndexType indexType) :
		m_indexType(indexType), m_nbVertices((uint32_t) numVertices), m_nbIndices((uint32_t) numIndices)
	{
		MOE_ASSERT(numVertices < UINT32_MAX && numIndices < UINT32_MAX); // assert if there was an overflow !

		auto vertexDataSize = numVertices * vertexSize;
		m_vertexBuffer = VulkanBuffer::NewVertexBuffer(device, vertexDataSize, vertexData);

		auto indexDataSize = numIndices;
		switch (indexType)
		{
		case vk::IndexType::eUint16:
			indexDataSize *= sizeof(uint16_t);
			break;
		case vk::IndexType::eUint32:
			indexDataSize *= sizeof(uint32_t);
			break;
		default:
			MOE_ASSERT(false); // not implemented
		}
		m_indexBuffer = VulkanBuffer::NewIndexBuffer(device, indexDataSize, indexData);
	}


	void VulkanMesh::Draw(vk::CommandBuffer drawCmdBuffer)
	{
		vk::DeviceSize offset = 0;
		vk::Buffer vertexBuffer = m_vertexBuffer.Handle();
		drawCmdBuffer.bindVertexBuffers(0, 1, &vertexBuffer, &offset);

		if (m_indexBuffer.Handle())
		{
			// TODO: add potential base buffer offset in the Vulkan Buffer
			drawCmdBuffer.bindIndexBuffer(m_indexBuffer.Handle(), 0, m_indexType);
			drawCmdBuffer.drawIndexed(m_nbIndices, 1, 0, 0, 0);
		}
		else
		{
			drawCmdBuffer.draw(m_nbVertices, 1, 0, 0);
		}
	}
}


#endif // MOE_VULKAN
