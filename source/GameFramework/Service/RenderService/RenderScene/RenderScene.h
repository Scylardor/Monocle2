// Monocle source files - Alexandre Baron

#pragma once
#include "RenderObject.h"
#include "ViewObject.h"
#include "Core/Containers/SparseArray/SparseArray.h"
#include "Core/Resource/MeshResource.h"
#include "Core/Resource/ResourceRef.h"
#include "Core/Resource/Material/MaterialResource.h"
#include "Graphics/RHI/BufferManager/BufferManager.h"
#include "Math/Vec3.h"


namespace moe
{
	class Renderer;

	class RenderScene
	{

	public:

		RenderScene(Renderer& sceneRenderer);


		RenderObjectHandle	AddObject( Ref<MeshResource> model, Ref<MaterialResource> material, Mat4 const& modelMatrix = Mat4::Identity());

		RenderObjectHandle	AddObject(DeviceMeshHandle meshHandle, DeviceMaterialHandle materialHandle, Mat4 const& modelMatrix = Mat4::Identity());


		using ViewID = uint32_t;
		ViewID	AddView(Mat4 const& view, PerspectiveProjectionData const& perspective,
			Rect2Df const& clipVp = ViewObject::FULLSCREEN_CLIP, Rect2Df const& clipSc = ViewObject::FULLSCREEN_CLIP);
		ViewID	AddView(Mat4 const& view, OrthographicProjectionData const& ortho,
			Rect2Df const& clipVp = ViewObject::FULLSCREEN_CLIP, Rect2Df const& clipSc = ViewObject::FULLSCREEN_CLIP);

		void	RegisterViewMatricesResources()
		{
			if (m_viewMatricesDelegateID == -1)
			{
				m_viewMatricesDelegateID = m_renderShaderChangeEvent.Add<RenderScene, &RenderScene::BindViewResourceSet>(this);
			}
		}

		void	BindViewResourceSet(RenderScene const& /*scene*/, ViewObject const& view, RenderQueue& drawQueue, RenderQueueKey key)
		{
			drawQueue.EmplaceDrawCall<CmdBindResourceSet>(key, view.GetMatricesResourceSetHandle());
		}


		float	OffsetViewFoVy(ViewID view, float FoVDelta, float min, float max);
		void	UpdateViewProjectionMatrix(ViewID vID, Mat4 const& newProj);
		void	UpdateViewMatrix(ViewID vID, Mat4 const& newView);


		void				RemoveObject(RenderObjectHandle handle);

		RenderObject const&	GetRenderObject(RenderObject::ID objID) const
		{
			return m_objects.Get(objID);
		}

		RenderObject &	MutRenderObject(RenderObject::ID objID)
		{
			return m_objects.Mut(objID);
		}

		struct TransformInfo
		{
			uint32_t		ID{ 0 };
			BufferBinding	TransformBufferBinding;
		};

		TransformInfo	AllocateTransform(Mat4 const& transform = Mat4::Identity());

		void		DeallocateTransform(uint32_t transfID);

		void		SetObjectTransform(uint32_t objectID, Mat4 const& newTransf);

		DeviceBufferHandle	GetTransformBufferHandle() const;

		DeviceBufferMapping const&	GetTransformBufferMapping() const
		{
			return m_transforms.Transforms;
		}


		SparseArray<ViewObject> const& GetViews() const
		{
			return m_views;
		}



		using RenderShaderChangeEvent = Event<void(RenderScene const&, ViewObject const&, RenderQueue&, RenderQueueKey)>;
		RenderShaderChangeEvent& OnRenderShaderChange() const
		{
			return m_renderShaderChangeEvent;
		}


		Renderer*	MutRenderer()
		{
			return m_sceneRenderer;
		}

		// For C++11 range for syntax
		auto	begin()
		{
			return m_objects.begin();
		}

		auto begin() const
		{
			return m_objects.begin();
		}

		auto end()
		{
			return m_objects.end();
		}

		auto end() const
		{
			return m_objects.end();
		}



	private:

		void	OnSurfaceResized(int width, int height);

		struct GPUTransforms
		{
			DeviceBufferMapping	Transforms;
			Vector<uint32_t>	AvailableTransforms;
			uint32_t			TransformsNumber = 0;

			void	Set(uint32_t transfID, Mat4 const& transf)
			{
				Transforms.MutBlock<Mat4>(transfID) = transf;
			}

		};

		GPUTransforms	m_transforms;

		Renderer*					m_sceneRenderer{ nullptr };

		SparseArray<RenderObject>	m_objects{};

		SparseArray<ViewObject>		m_views{};

		mutable RenderShaderChangeEvent	m_renderShaderChangeEvent{};
		EventDelegateID					m_viewMatricesDelegateID = (EventDelegateID)-1;


	};
}
