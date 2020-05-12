// Monocle Game Engine source files - Alexandre Baron

#include "RenderWorld.h"

#include "Graphics/Device/GraphicsDevice.h"

namespace moe
{

	Mesh* RenderWorld::CreateStaticMeshFromBuffer(const MeshDataDescriptor& vertexData,
		const MeshDataDescriptor& indexData)
	{
		if (vertexData.IsNull())
		{
			// No mesh data to process
			return nullptr;
		}

		DeviceBufferHandle vertexHandle = m_renderer.MutGraphicsDevice().CreateStaticVertexBuffer(vertexData.m_dataBuffer, vertexData.m_bufferSizeBytes);
		if (false == MOE_ASSERT(vertexHandle.IsNotNull()))
		{
			return nullptr;
		}

		DeviceBufferHandle indexHandle{0};

		if (false == indexData.IsNull())
		{
			indexHandle = m_renderer.MutGraphicsDevice().CreateIndexBuffer(indexData.m_dataBuffer, indexData.m_bufferSizeBytes);
			if (false == MOE_ASSERT(indexHandle.IsNotNull()))
			{
				// If creating the index buffer failed, don't forget to destroy the vertex buffer or it will go dangling
				m_renderer.MutGraphicsDevice().DeleteStaticVertexBuffer(vertexHandle);
				return nullptr;
			}
		}

		GraphicObjectData newMeshData{vertexHandle, indexHandle, DeviceBufferHandle::Null(), vertexData, indexData };

		FreelistID newMeshID = m_meshFreelist.Add(this, newMeshData);

		Mesh* newMesh = &m_meshFreelist.Lookup(newMeshID);

		newMesh->SetObjectID(newMeshID);

		m_activeObjects.PushBack(newMeshID);

		return newMesh;
	}


	void RenderWorld::DeleteStaticMesh(Mesh* mesh)
	{
		MOE_DEBUG_ASSERT(mesh != nullptr); // you're not supposed to pass null handles to this function
		if (mesh == nullptr)
			return;

		m_renderer.MutGraphicsDevice().DeleteStaticVertexBuffer(mesh->GetVertexBufferHandle());

		if (mesh->GetIndexBufferHandle().IsNotNull())
		{
			m_renderer.MutGraphicsDevice().DeleteIndexBuffer(mesh->GetIndexBufferHandle());
		}

		m_meshFreelist.Remove(mesh->GetID());
	}


	Camera* RenderWorld::CreateCamera(const OrthographicCameraDesc& orthoDesc, const ViewportDescriptor& vpDesc)
	{
		ViewportHandle vpHandle = m_renderer.MutGraphicsDevice().CreateViewport(vpDesc);
		return CreateCamera(orthoDesc, vpHandle);
	}


	Camera* RenderWorld::CreateCamera(const PerspectiveCameraDesc& perspDesc, const ViewportDescriptor& vpDesc)
	{
		ViewportHandle vpHandle = m_renderer.MutGraphicsDevice().CreateViewport(vpDesc);
		return CreateCamera(perspDesc, vpHandle);
	}


	Camera* RenderWorld::CreateCamera(const OrthographicCameraDesc& orthoDesc, ViewportHandle vpHandle)
	{
		auto cameraGpuHandle = m_renderer.CreateUniformBufferFrom(CameraGpuData());
		GraphicObjectData newCamData{cameraGpuHandle, sizeof(CameraGpuData)};

		auto cameraID = m_cameraManager.AddCamera(this, newCamData, vpHandle, orthoDesc);

		m_activeCameras.PushBack(cameraID);

		Camera& newCam = m_cameraManager.MutCamera(cameraID);
		newCam.SetObjectID(cameraID);

		return &newCam;
	}


	Camera* RenderWorld::CreateCamera(const PerspectiveCameraDesc& perspDesc, ViewportHandle vpHandle)
	{
		auto cameraGpuHandle = m_renderer.CreateUniformBufferFrom(CameraGpuData());
		GraphicObjectData newCamData{ cameraGpuHandle, sizeof(CameraGpuData) };

		auto cameraID = m_cameraManager.AddCamera(this, newCamData, vpHandle, perspDesc);

		m_activeCameras.PushBack(cameraID);

		Camera& newCam = m_cameraManager.MutCamera(cameraID);
		newCam.SetObjectID(cameraID);

		return &newCam;
	}


	void RenderWorld::UseCamera(Camera* cameraToUse)
	{
		if (cameraToUse == nullptr)
		{
			MOE_ERROR(ChanGraphics, "Null camera passed to %s.", __FUNCTION__);
			return;
		}

		// First, activate this camera's viewport
		auto vpHandle = cameraToUse->GetViewportHandle();
		m_renderer.MutGraphicsDevice().UseViewport(vpHandle);

		// Then update the camera GPU data if it has changed since last frame
		if (false == cameraToUse->IsTransformUpToDate())
		{
			const CameraGpuData& newCamData = cameraToUse->GetCameraGpuData();
			m_renderer.MutGraphicsDevice().UpdateBuffer(cameraToUse->GetCameraGpuHandle(), (const void*)&newCamData, sizeof(CameraGpuData));
			cameraToUse->SetTransformUpToDate();
		}

		m_currentCamera = cameraToUse;
	}


	void RenderWorld::DrawMesh(Mesh* drawnMesh, VertexLayoutHandle layoutHandle, Material* material)
	{
		if (drawnMesh == nullptr)
			return;

		if (material != nullptr)
		{
			m_renderer.UseMaterialPerObject(material, *drawnMesh);
		}

		DeviceBufferHandle vtxBufHandle = drawnMesh->GetVertexBufferHandle();
		DeviceBufferHandle idxBufHandle = drawnMesh->GetIndexBufferHandle();

		m_renderer.MutGraphicsDevice().DrawVertexBuffer(layoutHandle, vtxBufHandle, drawnMesh->NumVertices(), idxBufHandle, drawnMesh->NumIndices());

	}


	void RenderWorld::BeginDraw()
	{
		for (CameraManager::CameraID camID : m_activeCameras)
		{
			Camera* camera = &m_cameraManager.MutCamera(camID);


			// Activate the camera viewport
			UseCamera(camera);

			// Draw all objects
			for (FreelistID objID : m_activeObjects)
			{
				//Mesh& drawnMesh = m_meshFreelist.Lookup(objID);

				//drawnMesh.SetMaterial()
			}
		}
	}


	GraphicObjectData RenderWorld::ReallocObjectUniformGraphicData(const GraphicObjectData& oldData, uint32_t newNeededSize)
	{
		return m_renderer.ReallocObjectUniformGraphicData(oldData, newNeededSize);

	}
}
