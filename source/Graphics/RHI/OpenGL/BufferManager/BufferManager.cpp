#include "BufferManager.h"

#include "Graphics/Device/OpenGL/OpenGLGraphicsDevice.h"

namespace moe
{
	OpenGL4BufferManager::~OpenGL4BufferManager()
	{
		for (auto & [id, handle] : m_meshIDToHandle)
		{
			OpenGL4BufferManager::DestroyDeviceBuffer(handle.VertexBuffer);
		}
	}


	RenderMeshHandle OpenGL4BufferManager::FindOrCreateMeshBuffer(Ref<MeshResource> const& meshRsc)
	{
		// First, check if we don't already have that mesh somewhere
		auto [meshIt, inserted] = m_meshIDToHandle.TryEmplace(meshRsc.ID());
		if (inserted == false)
		{
			return meshIt->second; // already exists
		}

		// Not found : we have to create one
		// Allocate both vertex and index buffers in the same storage space,
		// Then separate them in two different handles.
		GLuint buffer;
		glCreateBuffers(1, &buffer);
		MOE_DEBUG_ASSERT(buffer != 0);

		// Compress both vertex and index data together
		size_t totalSize = meshRsc->Data.Vertices.Size() + meshRsc->Data.Indices.Size();
		byte_t* raw = new byte_t[totalSize];
		memcpy(raw, meshRsc->Data.Vertices.Data(), meshRsc->Data.Vertices.Size());
		memcpy(raw + meshRsc->Data.Vertices.Size(), meshRsc->Data.Indices.Data(), meshRsc->Data.Indices.Size());

		glNamedBufferStorage(buffer, totalSize, raw, GL_DYNAMIC_STORAGE_BIT);

		delete[] raw; // trash the copy

		// we have to make sure that we can store the offset: a buffer handle is a 64-bit value cut in two 32-bit values. So we cannot go above 2 << 32.
		MOE_ASSERT(meshRsc->Data.Vertices.Size() < moe::MaxValue<uint32_t>());

		// Now forge handles
		RenderMeshHandle meshHandle;
		meshHandle.VertexBuffer = OpenGLGraphicsDevice::EncodeBufferHandle(buffer, 0);
		meshHandle.IndexBuffer = OpenGLGraphicsDevice::EncodeBufferHandle(buffer, (uint32_t) meshRsc->Data.Vertices.Size());

		// Dont forget to update the lookup table
		meshIt->second = meshHandle;

		return meshHandle;
	}


	void OpenGL4BufferManager::DestroyDeviceBuffer(DeviceBufferHandle bufferToDestroy)
	{
		// decode the handle to fetch the buffer ID
		auto bufferID = OpenGLGraphicsDevice::DecodeBufferID(bufferToDestroy);
		glDeleteBuffers(1, &bufferID);
	}
}
