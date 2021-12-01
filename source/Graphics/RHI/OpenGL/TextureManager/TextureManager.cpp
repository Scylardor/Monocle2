#include "TextureManager.h"

namespace moe
{
	 DeviceTextureHandle OpenGL4TextureManager::CreateTexture2DFromFile(Ref<FileResource> const& /*textureFile*/)
	{
		 return DeviceTextureHandle::Null();
	}

	 DeviceTextureHandle OpenGL4TextureManager::CreateTexture2D()
	{
		 return DeviceTextureHandle::Null();
	}
}