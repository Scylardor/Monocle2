// Monocle source files - Alexandre Baron

#pragma once
#include "RenderObject.h"
#include "Core/Containers/SparseArray/SparseArray.h"
#include "Core/Resource/MeshResource.h"
#include "Core/Resource/ResourceRef.h"
#include "Core/Resource/Material/MaterialResource.h"


namespace moe
{
	class Renderer;

	class RenderScene
	{
	public:

		RenderScene(Renderer& sceneRenderer) :
			m_sceneRenderer(&sceneRenderer)
		{
		}

		RenderObjectHandle	AddObject( Ref<MeshResource> model, Ref<MaterialResource> material, Mat4 const& transform = Mat4::Identity());

		RenderObjectHandle	AddObject(DeviceMeshHandle meshHandle, DeviceMaterialHandle materialHandle, Mat4 const& transform = Mat4::Identity());

		RenderObject const&	GetRenderObject(RenderObject::ID objID) const
		{
			return m_objects.Get(objID);
		}

		RenderObject &	MutRenderObject(RenderObject::ID objID)
		{
			return m_objects.Mut(objID);
		}

	private:

		Renderer* m_sceneRenderer{ nullptr };

		SparseArray<RenderObject>	m_objects{};

	};
}
