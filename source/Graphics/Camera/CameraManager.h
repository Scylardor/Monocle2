// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Containers/FreeList/Freelist.h"

#include "Camera.h"

namespace moe
{
	class CameraManager
	{

	public:
		using CameraID = FreelistID;

		Monocle_Graphics_API CameraID	AddCamera(RenderWorld* world, const GraphicObjectData& data, ViewportHandle vpHandle, const OrthographicCameraDesc& orthoDesc);
		Monocle_Graphics_API CameraID	AddCamera(RenderWorld* world, const GraphicObjectData& data, ViewportHandle vpHandle, const PerspectiveCameraDesc& persDesc);

		[[nodiscard]] const Camera&	GetCamera(CameraID camID) const
		{
			return m_cameras.Lookup(camID);
		}


		[[nodiscard]] Camera&	MutCamera(CameraID camID)
		{
			return m_cameras.Lookup(camID);
		}


	private:
		Freelist<Camera>	m_cameras;
	};

}
