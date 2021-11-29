#pragma once

#ifndef __gl_h_
#include <glad/glad.h>
#endif

#include "Graphics/RHI/BufferManager/BufferManager.h"
#include "Monocle_Graphics_Export.h"
#include "Core/Containers/HashMap/HashMap.h"

namespace moe
{
	class OpenGL4BufferManager : public IBufferManager
	{
	public:

		~OpenGL4BufferManager() override;

		Monocle_Graphics_API RenderMeshHandle	FindOrCreateMeshBuffer(Ref<MeshResource> const& meshRsc) override;

		Monocle_Graphics_API void				DestroyDeviceBuffer(DeviceBufferHandle bufferToDestroy) override;

	private:

		DynamicObjectPool<GLuint>				m_bufferHandles{};
		HashMap< RegistryID, RenderMeshHandle>	m_meshIDToHandle{};
	};



}
