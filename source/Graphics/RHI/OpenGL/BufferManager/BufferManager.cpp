#include "BufferManager.h"

#include "Graphics/Device/OpenGL/OpenGLGraphicsDevice.h"
#include "../MaterialManager/MaterialManager.h"

namespace moe
{
	OpenGL4BufferManager::~OpenGL4BufferManager() noexcept
	{
		for (auto & [id, handle] : m_meshRscIDToHandle)
		{
			OpenGL4MeshData const& meshData = m_meshesData.Get(handle.Get());
			OpenGL4BufferManager::DestroyDeviceBuffer(meshData.VertexBuffer);
		}
	}


	DeviceMeshHandle OpenGL4BufferManager::FindOrCreateMeshBuffer(Ref<MeshResource> const& meshRsc)
	{
		// First, check if we don't already have that mesh somewhere
		auto [meshIt, inserted] = m_meshRscIDToHandle.TryEmplace(meshRsc.ID());
		if (inserted == false)
		{
			return meshIt->second; // already exists
		}

		// Not found : we have to create one
		DeviceMeshHandle meshHandle = CreateMesh(meshRsc->Data);

		// Dont forget to update the lookup table
		meshIt->second = meshHandle;

		return meshHandle;
	}


	DeviceMeshHandle OpenGL4BufferManager::FindOrCreateMeshBuffer(MeshData const& meshData)
	{
		// This is a "free mesh" version of the function that doesn't ask the lookup table if we know this mesh.
		// Careful because it may cause duplicates if used heavily. To avoid duplicates, we should use the Resource system.
		DeviceMeshHandle meshHandle = CreateMesh(meshData);
		return meshHandle;

	}


	void OpenGL4BufferManager::DestroyDeviceBuffer(DeviceBufferHandle bufferToDestroy)
	{
		// decode the handle to fetch the buffer ID
		auto bufferID = OpenGLGraphicsDevice::DecodeBufferID(bufferToDestroy);
		glDeleteBuffers(1, &bufferID);
	}


	void OpenGL4BufferManager::DrawMesh(OpenGL4MaterialManager const& materialManager, DeviceMeshHandle handle, DeviceMaterialHandle meshMaterial)
	{
		OpenGL4VertexLayout const& vtxLayout = materialManager.GetMaterialVertexLayout(meshMaterial);

		OpenGL4MeshData const& meshData = m_meshesData.Get(handle.Get());

		MOE_DEBUG_ASSERT(vtxLayout.Desc.BindingsLayout == LayoutType::Interleaved); // TODO: We should get rid of packed type soon
		auto [VBO, VBOoffset] = OpenGLGraphicsDevice::DecodeBufferHandle(meshData.VertexBuffer);
		glVertexArrayVertexBuffer(vtxLayout.VAO, 0, VBO,(GLintptr) VBOoffset, vtxLayout.TotalStride);

		if (meshData.IndexBuffer.IsNotNull())
		{
			auto [EBO, EBOoffset] = OpenGLGraphicsDevice::DecodeBufferHandle(meshData.IndexBuffer);

			glVertexArrayElementBuffer(vtxLayout.VAO, EBO);

			glDrawElements(vtxLayout.Topology, meshData.NumElements, meshData.ElementType, (const void*)((uint64_t)EBOoffset));
		}
		else
		{
			glDrawArrays(vtxLayout.Topology, 0, meshData.NumElements);
		}
	}


	DeviceMeshHandle OpenGL4BufferManager::CreateMesh(MeshData const& meshData)
	{
		// Allocate both vertex and index buffers in the same storage space,
		// Then separate them in two different handles.
		GLuint buffer;
		glCreateBuffers(1, &buffer);
		MOE_DEBUG_ASSERT(buffer != 0);

		// Compress both vertex and index data together
		size_t totalSize = meshData.Vertices.Size() + meshData.Indices.Size();
		byte_t* raw = new byte_t[totalSize];
		memcpy(raw, meshData.Vertices.Data(), meshData.Vertices.Size());
		memcpy(raw + meshData.Vertices.Size(), meshData.Indices.Data(), meshData.Indices.Size());

		glNamedBufferStorage(buffer, totalSize, raw, GL_DYNAMIC_STORAGE_BIT);

		delete[] raw; // trash the copy

		// we have to make sure that we can store the offset: a buffer handle is a 64-bit value cut in two 32-bit values. So we cannot go above 2 << 32.
		MOE_ASSERT(meshData.Vertices.Size() < moe::MaxValue<uint32_t>());

		// Now forge handles
		DeviceBufferHandle vbo = OpenGLGraphicsDevice::EncodeBufferHandle(buffer, 0);
		DeviceBufferHandle ebo = OpenGLGraphicsDevice::EncodeBufferHandle(buffer, (uint32_t)meshData.Vertices.Size());

		DeviceMeshHandle meshHandle(m_meshesData.Emplace(meshData, vbo, ebo));
		return meshHandle;
	}
}
