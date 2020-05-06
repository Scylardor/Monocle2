// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Containers/FreeList/Freelist.h"

#include "Camera.h"

#include "CameraHandle.h"


namespace moe
{
	class CameraManager
	{

	public:
		Monocle_Graphics_API CameraHandle	AddCamera(ViewportHandle vpHandle, const OrthographicCameraDesc& orthoDesc);
		Monocle_Graphics_API CameraHandle	AddCamera(ViewportHandle vpHandle, const PerspectiveCameraDesc& persDesc);

	private:
		Freelist<ACamera>	m_cameras;
	};

}
