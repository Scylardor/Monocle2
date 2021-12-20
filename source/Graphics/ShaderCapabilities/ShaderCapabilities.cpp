#include "ShaderCapabilities.h"

#include "Core/Resource/Material/MaterialResource.h"

#include "GameFramework/Service/RenderService/RenderScene/RenderScene.h"

#include "GameFramework/Service/RenderService/Renderer/Renderer.h"
#include "Graphics/RHI/RenderHardwareInterface.h"

namespace moe
{
	void SC_ObjectTransform::OnAdded(MaterialPassDescription& shaderPass)
	{
		shaderPass.ResourceSetLayouts.AddResourceLayout(
			(uint16_t) ReservedCapacitySets::OBJECT_TRANSFORMS,
			{ {0, BindingType::UniformBuffer, ShaderStage::Vertex} });
	}


	void SC_ObjectTransform::OnInitialized(RenderScene* scene, RenderObject& object)
	{
		uint32_t transfID = scene->AllocateTransform();
		object.SetTransformID(transfID);

		auto [bufID, bufOffset] = scene->GetTransformBufferHandle().DecodeBufferHandle();

		DeviceBufferHandle transfBufHandle = DeviceBufferHandle::Encode(bufID, bufOffset + sizeof(Mat4) * transfID);

		DeviceDynamicResourceSetHandle dynamicHandle =
			scene->MutRenderer()->MutRHI()->MaterialManager().AddDynamicBufferBinding(object.GetDynamicSetsHandle(),
			(uint32_t) ReservedCapacitySets::OBJECT_TRANSFORMS, 0, transfBufHandle, sizeof(Mat4));

		object.SetDynamicSetsHandle(dynamicHandle);

		object.RemovedEvent().Add<&SC_ObjectTransform::OnRemoved>();
	}


	void SC_ObjectTransform::OnRemoved(RenderScene* scene, RenderObject& object)
	{
		scene->DeallocateTransform(object.GetTransformID());
	}
}
