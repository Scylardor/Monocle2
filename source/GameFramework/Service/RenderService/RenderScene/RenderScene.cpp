#include "RenderScene.h"

#include <numeric>


#include "Graphics/RHI/RenderHardwareInterface.h"
#include "GameFramework/Service/RenderService/Renderer/Renderer.h"
#include "GameFramework/Service/RenderService/RenderService.h"
#include "GameFramework/Service/ConfigService/ConfigService.h"
#include "GameFramework/Engine/Engine.h"

#include "GameFramework/Service/RenderService/GraphicsSurface/GraphicsSurface.h"
#include "Graphics/ShaderCapabilities/ShaderCapabilities.h"

namespace moe
{
	RenderScene::RenderScene(Renderer& sceneRenderer):
		m_sceneRenderer(&sceneRenderer)
	{
		auto const* config = sceneRenderer.GetRenderService()->GetEngine()->GetService<ConfigService>();
		const int defaultSceneSize = config->GetInt("rendering.default_scene_size").value_or(32);

		m_transforms.AvailableTransforms.Resize(defaultSceneSize);
		// Push indices in reverse order so we can PopBack the first ones first.
		std::iota(m_transforms.AvailableTransforms.RBegin(), m_transforms.AvailableTransforms.REnd(), 0);
		m_transforms.Transforms = sceneRenderer.MutRHI()->BufferManager().MapCoherentDeviceBuffer(sizeof(Mat4), defaultSceneSize);

		m_transforms.TransformsNumber = defaultSceneSize;

		sceneRenderer.MutSurface()->OnSurfaceResizedEvent().Add<RenderScene, &RenderScene::OnSurfaceResized>(this);

	}

	RenderObjectHandle RenderScene::AddObject(Ref<MeshResource> model, Ref<MaterialResource> material, Mat4 const& modelMatrix)
	{
		RenderHardwareInterface* rhi = m_sceneRenderer->MutRHI();

		DeviceMeshHandle meshHandle = rhi->BufferManager().FindOrCreateMeshBuffer(model);

		DeviceMaterialHandle matHandle;
		if (material.IsValid())
		{
			matHandle = rhi->MaterialManager().CreateMaterial(material->GetDescription());
		}

		RenderObjectHandle handle = AddObject(meshHandle, matHandle, modelMatrix);
		RenderObject* object = handle.MutObject();

		// TODO: dealing with first pass only for now
		auto const& firstPass = material->GetDescription().PassDescriptors[0];
		for (ShaderCapability const& shaderCap : firstPass.ShaderCapabilities)
		{
			shaderCap.OnInitialized(this, *object);
		}

		return handle;
	}


	RenderObjectHandle RenderScene::AddObject(DeviceMeshHandle meshHandle, DeviceMaterialHandle materialHandle, Mat4 const& modelMatrix)
	{
		auto newObjID = m_objects.Emplace(meshHandle, materialHandle, modelMatrix);

		return RenderObjectHandle{ this, newObjID };
	}


	RenderScene::ViewID RenderScene::AddView(Mat4 const& view, PerspectiveProjectionData const& perspective,
		Rect2Df const& clipVp, Rect2Df const& clipSc)
	{
		auto surfDims = m_sceneRenderer->GetSurface()->GetDimensions();

		RenderHardwareInterface* rhi = m_sceneRenderer->MutRHI();

		ViewID newViewID = m_views.Emplace(rhi, view, perspective, surfDims, clipVp, clipSc);

		return newViewID;
	}

	RenderScene::ViewID RenderScene::AddView(Mat4 const& view, OrthographicProjectionData const& ortho,
		Rect2Df const& clipVp, Rect2Df const& clipSc)
	{
		auto surfDims = m_sceneRenderer->GetSurface()->GetDimensions();

		RenderHardwareInterface* rhi = m_sceneRenderer->MutRHI();

		ViewID newViewID = m_views.Emplace(rhi, view, ortho, surfDims, clipVp, clipSc);

		return newViewID;
	}


	float RenderScene::OffsetViewFoVy(ViewID view, float FoVDelta, float min, float max)
	{
		ViewObject& obj = m_views.Mut(view);
		return obj.OffsetPerspectiveFoVy(FoVDelta, min, max);
	}


	void RenderScene::UpdateViewProjectionMatrix(ViewID vID, Mat4 const& newProj)
	{
		ViewObject& view = m_views.Mut(vID);
		view.UpdateProjection(newProj);
	}


	void RenderScene::UpdateViewMatrix(ViewID vID, Mat4 const& newView)
	{
		ViewObject& view = m_views.Mut(vID);
		view.UpdateView(newView);
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


	RenderScene::TransformInfo RenderScene::AllocateTransform(Mat4 const& transform)
	{
		if (m_transforms.AvailableTransforms.Empty())
		{
			uint32_t prevTransfNumber = m_transforms.TransformsNumber;
			m_transforms.TransformsNumber *= 2;

			m_sceneRenderer->MutRHI()->BufferManager().ResizeMapping(m_transforms.Transforms, m_transforms.TransformsNumber);

			// Fill the free transform index list with all the newly allocated transforms.
			m_transforms.AvailableTransforms.Resize(prevTransfNumber);
			std::generate(m_transforms.AvailableTransforms.begin(), m_transforms.AvailableTransforms.end(),
				[n = m_transforms.TransformsNumber-1]() mutable { return n--; });
		}

		uint32_t nextTransformID = m_transforms.AvailableTransforms.Back();
		m_transforms.AvailableTransforms.PopBack();

		m_transforms.Set(nextTransformID, transform);

		auto [bufID, bufOffset] = m_transforms.Transforms.Handle().DecodeBufferHandle();

		auto off = bufOffset + m_transforms.Transforms.AlignedBlockSize() * nextTransformID;
		DeviceBufferHandle transfBufHandle = DeviceBufferHandle::Encode(bufID, off);

		BufferBinding transformBufferBinding{ transfBufHandle, 0, (uint32_t)ReservedCapacitySets::OBJECT_TRANSFORMS,
			off, sizeof(Mat4) };

		return TransformInfo{nextTransformID, transformBufferBinding};
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


	void RenderScene::OnSurfaceResized(int width, int height)
	{
		float fwidth = (float)width;
		float fheight = (float)height;
		float aspectRatio = fwidth / fheight;

		// Recompute all views of the scene as necessary because the drawing surface was rescaled.
		for (ViewObject& view : m_views)
		{
			view.UpdateViewport(fwidth, fheight, aspectRatio);
		}
	}
}
