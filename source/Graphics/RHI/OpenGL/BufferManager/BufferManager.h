#pragma once

#ifndef __gl_h_
#include <glad/glad.h>
#endif

#include "Graphics/RHI/BufferManager/BufferManager.h"
#include "Graphics/RHI/MaterialManager/MaterialManager.h"
#include "Monocle_Graphics_Export.h"
#include "Core/Containers/HashMap/HashMap.h"
#include "Graphics/Device/OpenGL/OpenGLGraphicsDevice.h"

namespace moe
{
	class OpenGL4MaterialManager;


	struct OpenGL4MeshData : TextureData
	{
		OpenGL4MeshData(MeshData const& data, DeviceBufferHandle vbo, DeviceBufferHandle ebo) :
			VertexBuffer(vbo), IndexBuffer(ebo)
		{
			if (IndexBuffer.IsNotNull())
			{
				if (data.ElementInformation == MeshElementType::eUint16)
				{
					ElementType = GL_UNSIGNED_SHORT;
					NumElements = (uint32_t) data.Indices.Size() / (uint32_t)sizeof(uint16_t);
				}
				else
				{
					ElementType = GL_UNSIGNED_INT;
					NumElements = (uint32_t)data.Indices.Size() / (uint32_t)sizeof(uint32_t);
				}
			}
			else
			{
				NumElements = (GLsizei)data.ElementInformation;
			}
		}

		GLsizei				NumElements{ 0 };
		GLenum				ElementType{ GL_UNSIGNED_INT };
		DeviceBufferHandle	VertexBuffer = DeviceBufferHandle::Null();
		DeviceBufferHandle	IndexBuffer = DeviceBufferHandle::Null();
	};


	class OpenGL4BufferManager : public IBufferManager
	{
	public:

		~OpenGL4BufferManager() noexcept override ;

		Monocle_Graphics_API DeviceMeshHandle	FindOrCreateMeshBuffer(Ref<MeshResource> const& meshRsc) override;

		Monocle_Graphics_API DeviceMeshHandle	FindOrCreateMeshBuffer(MeshData const& meshData) override;

		Monocle_Graphics_API void				DestroyDeviceBuffer(DeviceBufferHandle bufferToDestroy) override;


		GLuint	GetBuffer(DeviceBufferHandle handle)
		{
			GLuint bufferID = OpenGLGraphicsDevice::DecodeBufferID(handle);
			return bufferID;
		}


		void	DrawMesh(OpenGL4MaterialManager const& materialManager, DeviceMeshHandle handle, DeviceMaterialHandle meshMaterial);


	private:

		DeviceMeshHandle	CreateMesh(MeshData const& meshData);

		DynamicObjectPool<OpenGL4MeshData>		m_meshesData;

		HashMap<RegistryID, DeviceMeshHandle>	m_meshRscIDToHandle{};

	};



}
