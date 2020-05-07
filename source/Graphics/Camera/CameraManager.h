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

		[[nodiscard]] const ACamera&	GetCamera(CameraHandle camHandle) const
		{
			return m_cameras.Lookup(camHandle.Get() - 1);
		}

	private:
		Freelist<ACamera>	m_cameras;
	};

}
