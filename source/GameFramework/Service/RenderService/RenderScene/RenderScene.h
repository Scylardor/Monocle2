// Monocle source files - Alexandre Baron

#pragma once
#include "RenderObject.h"
#include "ViewObject.h"
#include "Core/Containers/SparseArray/SparseArray.h"
#include "Core/Resource/MeshResource.h"
#include "Core/Resource/ResourceRef.h"
#include "Core/Resource/Material/MaterialResource.h"
#include "Graphics/GraphicsSparseArray/GraphicsSparseArray.h"
#include "Graphics/RHI/BufferManager/BufferManager.h"
#include "Math/Vec3.h"
#include "GameFramework/Service/RenderService/RenderScene/LightObject.h"


namespace moe
{
	class Renderer;

	class RenderScene
	{

		template <typename T>
		struct GraphicsPool
		{
			GraphicsPool() = default;

			GraphicsPool(RenderHardwareInterface* RHI, uint32_t initialPoolSize)
			{
				AvailableIndices.Resize(initialPoolSize);
				// Push indices in reverse order so we can PopBack the first ones first.
				std::iota(AvailableIndices.RBegin(), AvailableIndices.REnd(), 0);
				PoolBuffer = RHI->BufferManager().MapCoherentDeviceBuffer(sizeof(T), initialPoolSize);

				Size = initialPoolSize;
			}


			[[nodiscard]] bool	IsFull() const
			{
				return AvailableIndices.Empty();
			}

			void	Grow(RenderHardwareInterface* RHI)
			{
				uint32_t prevSize = Size;
				Size *= 2;

				RHI->BufferManager().ResizeMapping(PoolBuffer, Size);

				// Fill the free index list with the indices of all the newly allocated cells.
				AvailableIndices.Resize(prevSize);
				std::generate(AvailableIndices.begin(), AvailableIndices.end(),
					[n = Size - 1]() mutable { return n--; });
			}

			uint32_t PopNextBlockID()
			{
				uint32_t NextBlockID = AvailableIndices.Back();
				AvailableIndices.PopBack();

				return NextBlockID;
			}


			uint32_t SetFirstAvailableBlock(const T& obj)
			{
				uint32_t nextBlock = PopNextBlockID();
				Set(nextBlock, obj);
				return nextBlock;
			}


			DeviceBufferMapping const& GetBufferMapping() const
			{
				return PoolBuffer;
			}


			T&	Mut(uint32_t id)
			{
				return PoolBuffer.MutBlock<T>(id);
			}

			DeviceBufferMapping	PoolBuffer;
			Vector<uint32_t>	AvailableIndices;
			uint32_t			Size = 0;

			void	Set(uint32_t idx, T const& obj)
			{
				PoolBuffer.MutBlock<T>(idx) = obj;
			}

			void	Free(uint32_t freed)
			{
				AvailableIndices.PushBack(freed);
			}
		};


	public:

		RenderScene(Renderer& sceneRenderer);


		RenderObjectHandle	AddObject( Ref<MeshResource> model, Ref<MaterialResource> material);

		RenderObjectHandle	AddObject(DeviceMeshHandle meshHandle, DeviceMaterialHandle materialHandle);


		using LightID = uint32_t;
		LightID	AddLight(DirectionalLight const& dir);
		LightID	AddLight(PointLight const& point);
		LightID	AddLight(SpotLight const& spot);

		auto	GetLightsNumber() const
		{
			return m_lights.GetSize();
		}

		using ViewID = uint32_t;
		ViewID	AddView(Mat4 const& view, PerspectiveProjectionData const& perspective,
			Rect2Df const& clipVp = ViewObject::FULLSCREEN_CLIP, Rect2Df const& clipSc = ViewObject::FULLSCREEN_CLIP);
		ViewID	AddView(Mat4 const& view, OrthographicProjectionData const& ortho,
			Rect2Df const& clipVp = ViewObject::FULLSCREEN_CLIP, Rect2Df const& clipSc = ViewObject::FULLSCREEN_CLIP);

		void	RegisterViewMatricesResources()
		{
			if (m_viewMatricesDelegateID == -1)
			{
				m_viewMatricesDelegateID = m_renderShaderChangeEvent.Add<&RenderScene::BindViewResourceSet>();
			}
		}

		static void	BindViewResourceSet(RenderScene const& /*scene*/, ViewObject const& view, RenderQueue& drawQueue, RenderQueueKey key)
		{
			drawQueue.EmplaceDrawCall<CmdBindResourceSet>(key, view.GetMatricesResourceSetHandle());
		}


		static void	BindLightsResourceSet(RenderScene const& scene, ViewObject const& /*view*/, RenderQueue& drawQueue, RenderQueueKey key)
		{
			if (scene.GetLightsNumber() != 0)
				drawQueue.EmplaceDrawCall<CmdBindResourceSet>(key, scene.GetLightsResourceSetHandle());

		}

		DeviceResourceSetHandle	GetLightsResourceSetHandle() const
		{
			return m_lightsResourceHandle;
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

		TransformInfo	AllocateTransform(Mat4 const& modelMatrix = Mat4::Identity());

		void		DeallocateTransform(uint32_t transfID);

		void		UpdateObjectViewMatrices(uint32_t objectID, Mat4 const& view, Mat4 const& viewProjection);
		void		UpdateObjectModel(uint32_t objectID, Mat4 const& model);

		DeviceBufferHandle	GetTransformBufferHandle() const;


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

		void	UpdateLightsResourceSet();

		void	OnSurfaceResized(int width, int height);

		struct ObjectMatrices
		{
			ObjectMatrices() = default;
			ObjectMatrices(Mat4 const& model) :
				MVP(model), ModelView(model), Model(model), Normal(model.GetInverseTransposed())
			{}

			ObjectMatrices(Mat4 const& model, Mat4 const& viewProj) :
				MVP(viewProj * model), ModelView(model), Model(model), Normal(model.GetInverseTransposed())
			{}

			Mat4	MVP{ Mat4::Identity() };
			Mat4	ModelView{ Mat4::Identity() };
			Mat4	Normal{ Mat4::Identity() };
			Mat4	Model{Mat4::Identity()};
		};


		GraphicsPool<ObjectMatrices>	m_transforms;

		Renderer*						m_sceneRenderer{ nullptr };

		SparseArray<RenderObject>			m_objects{};

		SparseArray<ViewObject>				m_views{};

		GraphicsSparseArray<LightObject>	m_lights;
		DeviceResourceSetHandle				m_lightsResourceHandle;

		mutable RenderShaderChangeEvent	m_renderShaderChangeEvent{};
		EventDelegateID					m_viewMatricesDelegateID = UINT64_MAX;


	};
}
