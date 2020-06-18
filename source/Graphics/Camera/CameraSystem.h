// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Containers/FreeList/Freelist.h"
#include "Core/Containers/Vector/Vector.h"
#include "Graphics/Device/GraphicsDevice.h"
#include "Graphics/DeviceBuffer/DeviceBufferHandle.h"
#include "Math/Vec4.h"

#include "Camera.h"

#include "Graphics/Material/MaterialBindings.h"

namespace moe
{

	class CameraSystem
	{

	public:
		Monocle_Graphics_API CameraSystem(IGraphicsDevice& device);
		Monocle_Graphics_API ~CameraSystem();


		Monocle_Graphics_API void	UpdateCameras();

		Monocle_Graphics_API void	BindCameraBuffer(uint32_t camIndex);

		Monocle_Graphics_API Camera*	AddNewCamera(ViewportHandle vpHandle, const PerspectiveCameraDesc& perspecDesc);
		Monocle_Graphics_API Camera*	AddNewCamera(ViewportHandle vpHandle, const OrthographicCameraDesc& cameraDesc);
		Monocle_Graphics_API Camera*	AddNewCamera(ViewportHandle vpHandle, const CameraData& camData, CameraProjection projType);
		Monocle_Graphics_API Camera*	AddNewCamera(ViewportHandle vpHandle, const CameraDescriptor& camDesc);


		Monocle_Graphics_API void		RemoveCamera(Camera* Camera);

		bool			NeedsUpdate() const { return m_needBufferUpdateFlag; }

		void			FlagUpdateNeeded() { m_needBufferUpdateFlag = true; }

		uint32_t		CamerasNumber() const { return m_nextCameraIdx; }

		size_t			GetCamerasBufferSizeBytes() const { return GetCameraDataSizeBytes() * m_CameraDataBuffer.Capacity(); }

		size_t			GetCameraDataSizeBytes() const { return sizeof(decltype(m_CameraDataBuffer)::ValueType); }


		DeviceBufferHandle	GetCamerasBufferHandle() const { return m_CamerasUniformBlock; }


		// C++11 range for interface
		Vector<Camera>::Iterator	begin()	{ return m_CameraObjects.Begin(); }
		Vector<Camera>::Iterator	end()	{ return m_CameraObjects.End(); }

		// TODO : check bounds
		[[nodiscard]] const Camera&	GetCamera(uint32_t camIndex) const { return m_CameraObjects[camIndex]; }


	private:

		IGraphicsDevice&	m_device;

		DeviceBufferHandle	m_CamerasUniformBlock;

		ResourceLayoutHandle	m_CamerasResourceLayout;

		ResourceSetHandle		m_CamerasResourceSet;

		Vector<CameraMatrices>	m_CameraDataBuffer;

		Vector<Camera>		m_CameraObjects;

		Vector<Camera*>		m_CamerasToUpdate;

		uint32_t	m_nextCameraIdx = 0;

		bool		m_needBufferUpdateFlag = false;

		static const int	ms_MAX_CAMERAS = 4;
	};

}
