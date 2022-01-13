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
		RenderScene::TransformInfo transformInfos = scene->AllocateTransform();

		object.SetTransformID(transformInfos.ID);
		DeviceDynamicResourceSetHandle dynamicHandle =
			scene->MutRenderer()->MutRHI()->MaterialManager().AddDynamicBufferBinding(object.GetDynamicSetsHandle(), transformInfos.TransformBufferBinding);

		object.SetDynamicSetsHandle(dynamicHandle);

		object.RemovedEvent().Add<&SC_ObjectTransform::OnRemoved>();
	}


	void SC_ObjectTransform::OnRemoved(RenderScene* scene, RenderObject& object)
	{
		scene->DeallocateTransform(object.GetTransformID());
	}


	void SC_ViewMatrices::OnAdded(MaterialPassDescription& shaderPass)
	{
		shaderPass.ResourceSetLayouts.AddResourceLayout(
			(uint16_t)ReservedCapacitySets::VIEW_MATRICES,
			{ {0, BindingType::UniformBuffer, ShaderStage::All} });
	}


	void SC_ViewMatrices::OnInitialized(RenderScene* scene, RenderObject& /*object*/)
	{
		scene->RegisterViewMatricesResources();
	}


	void SC_ViewMatrices::OnRemoved(RenderScene*, RenderObject&)
	{
	}


	void SC_PhongLighting::OnAdded(MaterialPassDescription& shaderPass)
	{
		shaderPass.ResourceSetLayouts.AddResourceLayout(
			(uint16_t)ReservedCapacitySets::SCENE_LIGHTS,
			{ {0, BindingType::StructuredBuffer, ShaderStage::Fragment} });

	}


	void SC_PhongLighting::OnInitialized(RenderScene*, RenderObject&)
	{

	}


	void SC_PhongLighting::OnRemoved(RenderScene*, RenderObject&)
	{

	}
}
