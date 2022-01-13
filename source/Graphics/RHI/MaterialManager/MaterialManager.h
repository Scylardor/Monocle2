#pragma once
#include "Graphics/Handle/DeviceHandles.h"
#include "Graphics/DeviceBuffer/DeviceBufferHandle.h"

namespace moe
{
	struct BufferBinding;
	struct MaterialDescription;
	struct PipelineDescription;
	struct ShaderProgramDescription;


	struct DeviceResourceSetHandle : RenderableObjectHandle<uint32_t>
	{
	private:
		static const Underlying ms_INVALID_ID = (Underlying)-1;

	public:

		DeviceResourceSetHandle(Underlying handleID = ms_INVALID_ID) :
			RenderableObjectHandle(handleID)
		{}

		static DeviceMeshHandle	Null() { return DeviceMeshHandle(); }
	};


	/*
	 * An abstract class for pipeline state object (PSO) management and resource set (aka descriptor set in e.g. Vulkan) allocation.
	 */
	class IMaterialManager
	{
	public:

		virtual ~IMaterialManager() = default;

		virtual bool	CreatePipelineStateObjectLayout(PipelineDescription const& pipeDesc) = 0;

		virtual bool	CreateShaderProgram(ShaderProgramDescription const& programDescription) = 0;

		virtual DeviceMaterialHandle	CreateMaterial(MaterialDescription const& matDesc) = 0;

		virtual DeviceDynamicResourceSetHandle	AddDynamicBufferBinding(
			DeviceDynamicResourceSetHandle dynamicSetHandle, BufferBinding const& dynamicBufferBinding) = 0;

		virtual void	FreeDynamicSets(DeviceDynamicResourceSetHandle freedHandle) = 0;

		virtual DeviceResourceSetHandle	AllocateResourceSet(struct ResourceSetsDescription const& setDescription) = 0;
		virtual void					UpdateResourceSet(DeviceResourceSetHandle handle, ResourceSetsDescription const& newDescription) = 0;
		virtual void					FreeResourceSet(DeviceResourceSetHandle setHandle) = 0;



	};


}
