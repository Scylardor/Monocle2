// Monocle source files - Alexandre Baron

#pragma once
#include "RenderObject.h"
#include "Core/Containers/SparseArray/SparseArray.h"
#include "Core/Resource/MeshResource.h"
#include "Core/Resource/ResourceRef.h"
#include "Core/Resource/Material/MaterialResource.h"
#include "Graphics/RHI/BufferManager/BufferManager.h"


namespace moe
{
	class Renderer;

	class RenderScene
	{

	public:

		RenderScene(Renderer& sceneRenderer);


		RenderObjectHandle	AddObject( Ref<MeshResource> model, Ref<MaterialResource> material, Mat4 const& transform = Mat4::Identity());

		RenderObjectHandle	AddObject(DeviceMeshHandle meshHandle, DeviceMaterialHandle materialHandle, Mat4 const& transform = Mat4::Identity());

		void				RemoveObject(RenderObjectHandle handle);

		RenderObject const&	GetRenderObject(RenderObject::ID objID) const
		{
			return m_objects.Get(objID);
		}

		RenderObject &	MutRenderObject(RenderObject::ID objID)
		{
			return m_objects.Mut(objID);
		}


		uint32_t	AllocateTransform(Mat4 const& transform = Mat4::Identity());

		void		DeallocateTransform(uint32_t transfID);

		void		SetObjectTransform(uint32_t objectID, Mat4 const& newTransf);

		DeviceBufferHandle	GetTransformBufferHandle() const;


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

		struct GPUTransforms
		{
			DeviceBufferMapping	Transforms;
			Vector<uint32_t>	AvailableTransforms;
			uint32_t			TransformsNumber = 0;

			void	Set(uint32_t transfID, Mat4 const& transf)
			{
				Transforms.As<Mat4*>()[transfID] = transf;
			}

		};

		GPUTransforms	m_transforms;

		Renderer*			m_sceneRenderer{ nullptr };

		SparseArray<RenderObject>	m_objects{};

	};
}
