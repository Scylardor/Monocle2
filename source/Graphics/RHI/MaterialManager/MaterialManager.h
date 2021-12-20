#pragma once
#include "Graphics/Handle/DeviceHandles.h"
#include "Graphics/DeviceBuffer/DeviceBufferHandle.h"

namespace moe
{
	struct MaterialDescription;
	struct PipelineDescription;
	struct ShaderProgramDescription;

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
			DeviceDynamicResourceSetHandle dynamicSetHandle, uint32_t setNumber, uint32_t bindingNumber, DeviceBufferHandle buf, uint32_t range) = 0;

		virtual void	FreeDynamicSets(DeviceDynamicResourceSetHandle freedHandle) = 0;



	};


}
