#include "RenderObject.h"
#include "RenderScene.h"


namespace moe
{
	const RenderObjectHandle	RenderObjectHandle::Null{ nullptr, 0 };

	RenderObject const* RenderObjectHandle::GetObject() const
	{
		if (m_scene == nullptr)
			return nullptr;

		return &m_scene->GetRenderObject(m_handle);
	}

	RenderObject* RenderObjectHandle::MutObject()
	{
		if (m_scene == nullptr)
			return nullptr;

		return &m_scene->MutRenderObject(m_handle);
	}
}