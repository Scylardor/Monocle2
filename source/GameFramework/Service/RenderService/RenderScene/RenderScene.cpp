#include "RenderScene.h"

#include "Graphics/RHI/RenderHardwareInterface.h"
#include "GameFramework/Service/RenderService/Renderer/Renderer.h"


namespace moe
{
	RenderObjectHandle RenderScene::AddObject(Ref<MeshResource> model, Ref<MaterialResource> material, Mat4 const& transform)
	{
		RenderHardwareInterface* rhi = m_sceneRenderer->MutRHI();

		RenderMeshHandle meshHandle = rhi->BufferManager().FindOrCreateMeshBuffer(model);

		DeviceMaterialHandle matHandle;
		if (material.IsValid())
		{
			matHandle = rhi->MaterialManager().CreateMaterial(material->GetDescription());
		}

		return AddObject(meshHandle, matHandle, transform);
	}


	RenderObjectHandle RenderScene::AddObject(RenderMeshHandle meshHandle, DeviceMaterialHandle materialHandle, Mat4 const& transform)
	{
		auto newObjID = m_objects.Emplace(meshHandle, materialHandle, transform);

		return RenderObjectHandle{ this, newObjID };
	}
}
