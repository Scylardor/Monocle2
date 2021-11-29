#include "RenderScene.h"

#include "Graphics/RHI/RenderHardwareInterface.h"
#include "GameFramework/Service/RenderService/Renderer/Renderer.h"


namespace moe
{
	RenderObjectHandle RenderScene::AddObject(Ref<MeshResource> model, Ref<MaterialResource> material, Mat4 const& transform)
	{
		RenderHardwareInterface* rhi = m_sceneRenderer->MutRHI();

		auto newObjID = m_objects.Emplace();

		RenderObject& object = m_objects.Mut(newObjID);
		object.SetModelMatrix(transform);

		rhi->BufferManager().FindOrCreateMeshBuffer(model);

		return RenderObjectHandle{ this, newObjID };
	}
}
