// Monocle Game Engine source files - Alexandre Baron

#ifdef MOE_OPENGL

#include "OpenGLRenderer.h"

#include <glad/glad.h>

namespace moe
{
	void OpenGLRenderer::Initialize()
	{
		m_device.Initialize();
	}


	void OpenGLRenderer::Shutdown()
	{
		m_device.Destroy();
	}


	MeshHandle OpenGLRenderer::CreateStaticMeshFromBuffer(const MeshDataDescriptor& vertexData, const MeshDataDescriptor& indexData)
	{
		if (vertexData.IsNull())
		{
			// No mesh data to process
			return MeshHandle::Null();
		}

		VertexBufferHandle vertexHandle = m_device.CreateStaticVertexBuffer(vertexData.m_dataBuffer, vertexData.m_bufferSizeBytes);
		if (false == MOE_ASSERT(!vertexHandle.IsNull()))
		{
			return MeshHandle::Null();
		}

		IndexBufferHandle indexHandle = IndexBufferHandle::Null();

		if (false == indexData.IsNull())
		{
			indexHandle = m_device.CreateIndexBuffer(indexData.m_dataBuffer, indexData.m_bufferSizeBytes);
			if (false == MOE_ASSERT(!indexHandle.IsNull()))
			{
				// If creating the index buffer failed, don't forget to destroy the vertex buffer or it will go dangling
				m_device.DeleteStaticVertexBuffer(vertexHandle);
				return MeshHandle::Null();
			}
		}

		auto meshIt = m_meshStorage.insert(Mesh{ vertexHandle, vertexData.m_bufferNumElems, indexHandle, indexData.m_bufferNumElems });
		MOE_ASSERT(meshIt.second); // We don't expect being able to insert twice the same mesh !

		return MeshHandle{(uint32_t)m_meshStorage.size()-1};
	}


	bool OpenGLRenderer::SetupGraphicsContext(GraphicsContextSetup setupFunc)
	{
		return gladLoadGLLoader((GLADloadproc)setupFunc);
	}
}

#endif // MOE_OPENGL