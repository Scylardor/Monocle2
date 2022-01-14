#include "RenderScene.h"

#include <numeric>


#include "Graphics/RHI/RenderHardwareInterface.h"
#include "GameFramework/Service/RenderService/Renderer/Renderer.h"
#include "GameFramework/Service/RenderService/RenderService.h"
#include "GameFramework/Service/ConfigService/ConfigService.h"
#include "GameFramework/Engine/Engine.h"

#include "GameFramework/Service/RenderService/GraphicsSurface/GraphicsSurface.h"
#include "Graphics/ShaderCapabilities/ShaderCapabilities.h"
#include "Math/Vec4.h"

namespace moe
{
	RenderScene::RenderScene(Renderer& sceneRenderer):
		m_sceneRenderer(&sceneRenderer)
	{
		auto const* config = sceneRenderer.GetRenderService()->GetEngine()->GetService<ConfigService>();

		const int defaultSceneSize = config->GetInt("rendering.default_scene_size").value_or(32);
		m_transforms = GraphicsPool<ObjectMatrices>(sceneRenderer.MutRHI(), defaultSceneSize);

		const int defaultNumLights = config->GetInt("rendering.default_scene_lights").value_or(32);
		m_lights.Initialize(m_sceneRenderer->MutRHI(), defaultNumLights);

		ResourceSetsDescription lightsSetDesc;
		lightsSetDesc.EmplaceBinding<BufferBinding>(m_lights.GetDeviceHandle(), (int)ReservedCapacitySets::SCENE_LIGHTS, 0, 0, m_lights.GetDataBytesRange(), BindingType::StructuredBuffer);
		m_lightsResourceHandle = sceneRenderer.MutRHI()->MaterialManager().AllocateResourceSet(lightsSetDesc);
		OnRenderShaderChange().Add<&RenderScene::BindLightsResourceSet>();

		sceneRenderer.MutSurface()->OnSurfaceResizedEvent().Add<RenderScene, &RenderScene::OnSurfaceResized>(this);

	}

	RenderObjectHandle RenderScene::AddObject(Ref<MeshResource> model, Ref<MaterialResource> material)
	{
		RenderHardwareInterface* rhi = m_sceneRenderer->MutRHI();

		DeviceMeshHandle meshHandle = rhi->BufferManager().FindOrCreateMeshBuffer(model);

		DeviceMaterialHandle matHandle;
		if (material.IsValid())
		{
			matHandle = rhi->MaterialManager().CreateMaterial(material->GetDescription());
		}

		RenderObjectHandle handle = AddObject(meshHandle, matHandle);
		RenderObject* object = handle.MutObject();

		// TODO: dealing with first pass only for now
		auto const& firstPass = material->GetDescription().PassDescriptors[0];
		for (ShaderCapability const& shaderCap : firstPass.ShaderCapabilities)
		{
			shaderCap.OnInitialized(this, *object);
		}

		return handle;
	}


	RenderObjectHandle RenderScene::AddObject(DeviceMeshHandle meshHandle, DeviceMaterialHandle materialHandle)
	{
		auto newObjID = m_objects.Emplace(meshHandle, materialHandle);

		return RenderObjectHandle{ this, newObjID };
	}


	RenderScene::LightID RenderScene::AddLight(DirectionalLight const& dir)
	{
		auto id = m_lights.New();
		LightObject& newLight = m_lights.Mut(id);

		newLight.Direction = dir.Direction;
		newLight.Ambient = dir.Ambient;
		newLight.Diffuse = dir.Diffuse;
		newLight.Specular = dir.Specular;

		// To let the shader know this is a directional light
		newLight.ConstantAttenuation = LightObject::DIRECTIONAL_ATTENUATION;

		UpdateLightsResourceSet();

		return id;
	}


	RenderScene::LightID RenderScene::AddLight(PointLight const& point)
	{
		auto id = m_lights.New();
		LightObject& newLight = m_lights.Mut(id);

		newLight.Position = point.Position;

		newLight.Ambient = point.Ambient;
		newLight.Diffuse = point.Diffuse;
		newLight.Specular = point.Specular;

		newLight.ConstantAttenuation = point.ConstantAttenuation;
		newLight.LinearAttenuation = point.LinearAttenuation;
		newLight.QuadraticAttenuation = point.QuadraticAttenuation;

		// To let the shader know this is a point light
		newLight.SpotInnerCutoff = LightObject::OMNIDIRECTIONAL_CUTOFF;

		UpdateLightsResourceSet();

		return id;
	}


	RenderScene::LightID RenderScene::AddLight(SpotLight const& spot)
	{
		auto id = m_lights.New();
		LightObject& newLight = m_lights.Mut(id);

		newLight.Position = spot.Position;
		newLight.Direction = spot.Direction;

		newLight.Ambient = spot.Ambient;
		newLight.Diffuse = spot.Diffuse;
		newLight.Specular = spot.Specular;

		newLight.ConstantAttenuation = spot.ConstantAttenuation;
		newLight.LinearAttenuation = spot.LinearAttenuation;
		newLight.QuadraticAttenuation = spot.QuadraticAttenuation;

		MOE_DEBUG_ASSERT(spot.InnerCutoff >= 0 && spot.OuterCutoff >= 0);
		newLight.SpotInnerCutoff = std::cosf(spot.InnerCutoff);
		newLight.SpotOuterCutoff = std::cosf(spot.OuterCutoff);

		UpdateLightsResourceSet();

		return id;
	}


	void RenderScene::UpdateLightPosition(LightID lightID, Vec3 const& newPos)
	{
		LightObject& light = m_lights.Mut(lightID);
		light.Position = newPos;
	}


	void RenderScene::UpdateLightTransform(LightID lightID, Vec3 const& newPos, Vec3 const& newDir)
	{
		LightObject& light = m_lights.Mut(lightID);
		light.Position = newPos;
		light.Direction = newDir;
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


	void RenderScene::UpdateProjectionMatrix(ViewID vID, Mat4 const& newProj)
	{
		ViewObject& view = m_views.Mut(vID);
		view.UpdateProjection(newProj);
	}


	void RenderScene::UpdateViewMatrix(ViewID vID, Mat4 const& newView, Vec3 const& newPos)
	{
		ViewObject& view = m_views.Mut(vID);
		view.UpdateView(newView, newPos);
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


	RenderScene::TransformInfo RenderScene::AllocateTransform(Mat4 const& modelMatrix)
	{
		if (m_transforms.IsFull())
		{
			m_transforms.Grow(m_sceneRenderer->MutRHI());
		}

		uint32_t nextTransformID = m_transforms.SetFirstAvailableBlock(modelMatrix);

		auto [bufID, bufOffset] = m_transforms.GetBufferMapping().Handle().DecodeBufferHandle();

		auto off = bufOffset + m_transforms.GetBufferMapping().AlignedBlockSize() * nextTransformID;
		DeviceBufferHandle transfBufHandle = DeviceBufferHandle::Encode(bufID, off);

		BufferBinding transformBufferBinding{ transfBufHandle, 0, (uint32_t)ReservedCapacitySets::OBJECT_TRANSFORMS,
			off, sizeof(Mat4) };

		return TransformInfo{nextTransformID, transformBufferBinding};
	}


	void RenderScene::DeallocateTransform(uint32_t transfID)
	{
		m_transforms.Free(transfID);
	}


	void RenderScene::UpdateObjectViewMatrices(uint32_t objectID, Mat4 const& view, Mat4 const& viewProjection)
	{
		auto transfID = m_objects.Get(objectID).GetTransformID();

		ObjectMatrices& objMats = m_transforms.Mut(transfID);
		objMats.MVP = viewProjection * objMats.Model;
		objMats.ModelView = view * objMats.Model;
	}


	void RenderScene::UpdateObjectModel(uint32_t objectID, Mat4 const& model)
	{
		auto transfID = m_objects.Get(objectID).GetTransformID();

		ObjectMatrices& objMats = m_transforms.Mut(transfID);
		objMats.Model = model;
	}


	DeviceBufferHandle RenderScene::GetTransformBufferHandle() const
	{
		return m_transforms.GetBufferMapping().Handle();

	}


	void RenderScene::UpdateLightsResourceSet()
	{
		ResourceSetsDescription lightsSetDesc;
		lightsSetDesc.EmplaceBinding<BufferBinding>(m_lights.GetDeviceHandle(), (int)ReservedCapacitySets::SCENE_LIGHTS, 0, 0, sizeof(LightObject) * m_lights.GetSize(), BindingType::StructuredBuffer);

		m_sceneRenderer->MutRHI()->MaterialManager().UpdateResourceSet(m_lightsResourceHandle, lightsSetDesc);
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
