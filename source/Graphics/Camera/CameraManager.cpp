// Monocle Game Engine source files - Alexandre Baron

#include "CameraManager.h"

namespace moe
{
	CameraManager::CameraID CameraManager::AddCamera(RenderWorld* world, const GraphicObjectData& data, ViewportHandle vpHandle, const OrthographicCameraDesc& orthoDesc)
	{
		return m_cameras.Add(world, data, vpHandle, orthoDesc);
	}


	CameraManager::CameraID CameraManager::AddCamera(RenderWorld* world, const GraphicObjectData& data, ViewportHandle vpHandle, const PerspectiveCameraDesc& persDesc)
	{
		return m_cameras.Add(world, data, vpHandle, persDesc);
	}
}
