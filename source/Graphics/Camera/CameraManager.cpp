// Monocle Game Engine source files - Alexandre Baron

#include "CameraManager.h"

namespace moe
{
	CameraHandle CameraManager::AddCamera(ViewportHandle vpHandle, const OrthographicCameraDesc& orthoDesc)
	{
		FreelistID newCamId = m_cameras.Add(vpHandle, orthoDesc);
		return newCamId.ToHandle<CameraHandle>();
	}


	CameraHandle CameraManager::AddCamera(ViewportHandle vpHandle, const PerspectiveCameraDesc& persDesc)
	{
		FreelistID newCamId = m_cameras.Add(vpHandle, persDesc);
		return newCamId.ToHandle<CameraHandle>();
	}
}
