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


	DeviceBufferMapping OpenGL4BufferManager::MapCoherentDeviceBuffer(uint32_t dataBlockSize, uint32_t numBlocks, void const* data,
		bool alignUniform, uint32_t mappingOffset, size_t mappingRange)
	{
		constexpr GLbitfield
			mapping_flags = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT,
			storage_flags = GL_DYNAMIC_STORAGE_BIT | mapping_flags;

		// in order to be able to bind subparts of the buffer,
		// the block size will have to be aligned to a multiple of GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT.
		// see https://community.khronos.org/t/uniform-buffers-and-bindbufferrange/65397/2

		static const GLint uboOffsetAlignment = GetUniformBufferOffsetAlignment();

		if (alignUniform)
			dataBlockSize = (dataBlockSize + uboOffsetAlignment - 1) & ~(uboOffsetAlignment - 1);

		auto totalSize = dataBlockSize * numBlocks;

		if (mappingRange == DeviceBufferMapping::WHOLE_RANGE)
			mappingRange = totalSize;

		GLuint buffer;
		glCreateBuffers(1, &buffer);
		MOE_DEBUG_ASSERT(buffer != 0);

		glNamedBufferStorage(buffer, totalSize, data, storage_flags);
		void* ptr = glMapNamedBufferRange(buffer, mappingOffset, mappingRange, mapping_flags);

		DeviceBufferHandle buffHandle = OpenGLGraphicsDevice::EncodeBufferHandle(buffer, mappingOffset);

		ObjectPoolID mappingID = m_mappings.Emplace(buffHandle, mappingRange);

		DeviceBufferMapping mapping{ mappingID, buffHandle, ptr, dataBlockSize };
		return mapping;
	}


	void OpenGL4BufferManager::Unmap(DeviceBufferMapping const& bufferMap)
	{
		auto const& mapping = m_mappings.Get(bufferMap.MappingID());

		GLuint buffer = OpenGLGraphicsDevice::DecodeBufferID(mapping.BufferHandle);
		glUnmapNamedBuffer(buffer);
		glDeleteBuffers(1, &buffer);

		m_mappings.Free(bufferMap.MappingID());
	}


	void OpenGL4BufferManager::ResizeMapping(DeviceBufferMapping & bufferMap, uint32_t newSize)
	{
		// First unmap the original buffer
		auto & buffMapping = m_mappings.Mut(bufferMap.MappingID());
		auto [srcBuffer, srcOffset] = OpenGLGraphicsDevice::DecodeBufferHandle(buffMapping.BufferHandle);
		glUnmapNamedBuffer(srcBuffer);

		// Create the new one
		constexpr GLbitfield
			mapping_flags = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT,
			storage_flags = GL_DYNAMIC_STORAGE_BIT | mapping_flags;

		GLuint destBuffer;
		glCreateBuffers(1, &destBuffer);
		MOE_DEBUG_ASSERT(destBuffer != 0);
		glNamedBufferStorage(destBuffer, bufferMap.AlignedBlockSize() * newSize, nullptr, storage_flags);

		// Make the copy from one buffer to another

		// If not using OpenGL 4.5, you need to use something like this :
		//glBindBuffer(GL_COPY_READ_BUFFER, vbo1);
		//glBindBuffer(GL_COPY_WRITE_BUFFER, vbo2);
		glCopyNamedBufferSubData(srcBuffer, destBuffer, srcOffset, 0, buffMapping.Range);

		// Delete the old buffer (we don't need it anymore)
		glDeleteBuffers(1, &srcBuffer);

		// Map the new buffer.
		buffMapping.BufferHandle = OpenGLGraphicsDevice::EncodeBufferHandle(destBuffer, 0);
		buffMapping.Range = newSize;

		void* ptr = glMapNamedBufferRange(destBuffer, 0, newSize, mapping_flags);
		MOE_ASSERT(ptr != nullptr);

		bufferMap = DeviceBufferMapping(bufferMap.MappingID(), buffMapping.BufferHandle, ptr, bufferMap.AlignedBlockSize());
	}


	void OpenGL4BufferManager::DrawMesh(DeviceMeshHandle handle, OpenGL4VertexLayout const* vtxLayout)
	{
		MOE_ASSERT(vtxLayout != nullptr);

		OpenGL4MeshData const& meshData = m_meshesData.Get(handle.Get());

		MOE_DEBUG_ASSERT(vtxLayout->Desc.BindingsLayout == LayoutType::Interleaved); // TODO: We should get rid of packed type soon
		auto [VBO, VBOoffset] = OpenGLGraphicsDevice::DecodeBufferHandle(meshData.VertexBuffer);
		glVertexArrayVertexBuffer(vtxLayout->VAO, 0, VBO,(GLintptr) VBOoffset, vtxLayout->TotalStride);

		if (meshData.IndexBuffer.IsNotNull())
		{
			auto [EBO, EBOoffset] = OpenGLGraphicsDevice::DecodeBufferHandle(meshData.IndexBuffer);

			glVertexArrayElementBuffer(vtxLayout->VAO, EBO);

			glDrawElements(vtxLayout->Topology, meshData.NumElements, meshData.ElementType, (const void*)((uint64_t)EBOoffset));
		}
		else
		{
			glDrawArrays(vtxLayout->Topology, 0, meshData.NumElements);
		}
	}


	uint32_t OpenGL4BufferManager::GetUniformBufferOffsetAlignment()
	{
		GLint uniformBufferAlignSize = 0;
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferAlignSize);
		return uniformBufferAlignSize;
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

		DeviceBufferHandle ebo;
		if (!meshData.Indices.Empty())
		{
			ebo = OpenGLGraphicsDevice::EncodeBufferHandle(buffer, (uint32_t)meshData.Vertices.Size());
		}

		DeviceMeshHandle meshHandle(m_meshesData.Emplace(meshData, vbo, ebo));
		return meshHandle;
	}
}
