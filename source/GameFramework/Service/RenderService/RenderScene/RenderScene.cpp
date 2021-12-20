#include "RenderScene.h"

#include <numeric>


#include "Graphics/RHI/RenderHardwareInterface.h"
#include "GameFramework/Service/RenderService/Renderer/Renderer.h"
#include "GameFramework/Service/RenderService/RenderService.h"
#include "GameFramework/Service/ConfigService/ConfigService.h"
#include "GameFramework/Engine/Engine.h"

namespace moe
{
	RenderScene::RenderScene(Renderer& sceneRenderer):
		m_sceneRenderer(&sceneRenderer)
	{
		auto const* config = sceneRenderer.GetRenderService()->GetEngine()->GetService<ConfigService>();
		const int defaultSceneSize = config->GetInt("rendering.default_scene_size").value_or(32);

		m_transforms.AvailableTransforms.Resize(defaultSceneSize);
		std::iota(m_transforms.AvailableTransforms.RBegin(), m_transforms.AvailableTransforms.REnd(), 0);
		m_transforms.Transforms = sceneRenderer.MutRHI()->BufferManager().MapCoherentDeviceBuffer(sizeof(Mat4) * defaultSceneSize);

		m_transforms.TransformsNumber = defaultSceneSize;

	}

	RenderObjectHandle RenderScene::AddObject(Ref<MeshResource> model, Ref<MaterialResource> material, Mat4 const& transform)
	{
		RenderHardwareInterface* rhi = m_sceneRenderer->MutRHI();

		DeviceMeshHandle meshHandle = rhi->BufferManager().FindOrCreateMeshBuffer(model);

		DeviceMaterialHandle matHandle;
		if (material.IsValid())
		{
			matHandle = rhi->MaterialManager().CreateMaterial(material->GetDescription());
		}

		RenderObjectHandle handle = AddObject(meshHandle, matHandle, transform);
		RenderObject* object = handle.MutObject();

		// TODO: dealing with first pass only for now
		auto const& firstPass = material->GetDescription().PassDescriptors[0];
		for (ShaderCapability const& shaderCap : firstPass.ShaderCapabilities)
		{
			shaderCap.OnInitialized(this, *object);
		}

		return handle;
	}


	RenderObjectHandle RenderScene::AddObject(DeviceMeshHandle meshHandle, DeviceMaterialHandle materialHandle, Mat4 const& transform)
	{
		auto newObjID = m_objects.Emplace(meshHandle, materialHandle, transform);

		return RenderObjectHandle{ this, newObjID };
	}


	void RenderScene::RemoveObject(RenderObjectHandle handle)
	{
		RenderObject& object = MutRenderObject(handle.GetID());

		// Free any dynamic resource set attached
		if (object.GetDynamicSetsHandle() != INVALID_ID)
		{
			MutRenderer()->MutRHI()->MaterialManager().FreeDynamicSets(object.GetDynamicSetsHandle());
		}

		// Fire the destroy event
		object.RemovedEvent().Broadcast(this, object);

		// Finally, destroy the object
		m_objects.Remove(handle.GetID());
	}


	uint32_t RenderScene::AllocateTransform(Mat4 const& transform)
	{
		if (m_transforms.AvailableTransforms.Empty())
		{
			uint32_t prevTransfNumber = m_transforms.TransformsNumber;
			m_transforms.TransformsNumber *= 2;

			m_sceneRenderer->MutRHI()->BufferManager().ResizeMapping(m_transforms.Transforms, sizeof(Mat4) * m_transforms.TransformsNumber);

			std::iota(m_transforms.AvailableTransforms.RBegin(), m_transforms.AvailableTransforms.REnd(), prevTransfNumber);
		}

		uint32_t nextTransformID = m_transforms.AvailableTransforms.Back();
		m_transforms.AvailableTransforms.PopBack();

		m_transforms.Set(nextTransformID, transform);

		return nextTransformID;
	}


	void RenderScene::DeallocateTransform(uint32_t transfID)
	{
		m_transforms.AvailableTransforms.PushBack(transfID);
	}


	void RenderScene::SetObjectTransform(uint32_t objectID, Mat4 const& newTransf)
	{
		auto transfID = m_objects.Get(objectID).GetTransformID();
		m_transforms.Set(transfID, newTransf);
	}


	DeviceBufferHandle RenderScene::GetTransformBufferHandle() const
	{
		return m_transforms.Transforms.Handle();

	}
}
