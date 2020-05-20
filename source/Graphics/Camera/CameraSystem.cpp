// Monocle Game Engine source files - Alexandre Baron

#include "CameraSystem.h"

#include "Graphics/Resources/ResourceLayout/ResourceLayoutDescriptor.h"
#include "Graphics/Resources/ResourceSet/ResourceSetDescriptor.h"

#include "Graphics/Material/MaterialBindings.h"

namespace moe
{
	CameraSystem::CameraSystem(IGraphicsDevice& device) :
		m_device(device)
	{
		m_CameraObjects.Reserve(ms_MAX_CAMERAS);
		m_CameraDataBuffer.Resize(ms_MAX_CAMERAS);

		/* TODO : Creating layout here is probably bad... + make the shader stage parameterized */
		const ResourceLayoutDescriptor cameraLayoutDesc{
			{{ "CameraMatrices",  MaterialBlockBinding::VIEW_CAMERA, ResourceKind::UniformBuffer, ShaderStage(ShaderStage::Vertex | ShaderStage::Fragment) }}
		};

		m_CamerasResourceLayout = m_device.CreateResourceLayout(cameraLayoutDesc);

		m_CamerasUniformBlock = device.CreateUniformBuffer(
			m_CameraDataBuffer.Data(), GetCamerasBufferSizeBytes());

		ResourceSetDescriptor CameraSetDesc{
			m_CamerasResourceLayout,
			{m_CamerasUniformBlock}
		};

		m_CamerasResourceSet = m_device.CreateResourceSet(CameraSetDesc);
	}


	CameraSystem::~CameraSystem()
	{
		/* TODO : free uniform buffer */
	}


	void CameraSystem::UpdateCameras()
	{
		if (!NeedsUpdate())
			return; // nothing to do

		// Don't update the full buffer but only up to the number of Cameras currently in fCamera.
		m_device.UpdateBuffer(m_CamerasUniformBlock, m_CameraDataBuffer.Data(), CamerasNumber() * GetCameraDataSizeBytes());

		// buffer is updated
		m_needBufferUpdateFlag = false;
	}


	void CameraSystem::BindCameraBuffer(uint32_t camIndex)
	{
		Camera& currentCam = m_CameraObjects[camIndex]; // TODO: check bounds
		// First, activate this camera's viewport
		auto vpHandle = currentCam.GetViewportHandle();
		m_device.UseViewport(vpHandle);

		m_device.BindUniformBlock(MaterialBlockBinding::VIEW_CAMERA, m_CamerasUniformBlock, sizeof(CameraMatrices), camIndex * sizeof(CameraMatrices));
	}


	Camera* CameraSystem::AddNewCamera(ViewportHandle vpHandle, const PerspectiveCameraDesc& perspecDesc)
	{
		return AddNewCamera(vpHandle, CameraData(perspecDesc), CameraProjection::Perspective);
	}


	Camera* CameraSystem::AddNewCamera(ViewportHandle vpHandle, const OrthographicCameraDesc& orthoDesc)
	{
		return AddNewCamera(vpHandle, CameraData(orthoDesc), CameraProjection::Orthographic);
	}


	Camera* CameraSystem::AddNewCamera(ViewportHandle vpHandle, const CameraData& camData, CameraProjection projType)
	{
		MOE_ASSERT(m_CameraObjects.Size() < ms_MAX_CAMERAS);

		m_needBufferUpdateFlag = true; // Will resend Cameras buffer.

		m_CameraObjects.EmplaceBack(this, vpHandle, camData, projType, &m_CameraDataBuffer[m_nextCameraIdx]);

		return &m_CameraObjects[m_nextCameraIdx++];
	}


	void CameraSystem::RemoveCamera(Camera* Camera)
	{
		auto swappedCameraIt = m_CameraObjects.EraseBySwapAt(Camera->GetCameraIndex());

		if (swappedCameraIt != m_CameraObjects.End())
		{
			// Put the swapped Camera data inside the removed Camera slot
			m_CameraDataBuffer[Camera->GetCameraIndex()] = m_CameraDataBuffer[swappedCameraIt->GetCameraIndex()];

			// Don't forget to update the swapped Camera index !
			swappedCameraIt->SetCameraIndex(Camera->GetCameraIndex());
		}

		m_nextCameraIdx--; // One Camera has been removed : decrease the next index.

		m_needBufferUpdateFlag = true; // Will resend Cameras buffer.
	}
}
