#pragma once
#include "Core/Resource/Material/MaterialResource.h"

namespace moe
{

	struct DeviceMaterialHandle : RenderableObjectHandle<std::uint64_t>
	{
	private:
		static const Underlying ms_INVALID_ID = (Underlying)-1;

	public:

		DeviceMaterialHandle(Underlying handleID = ms_INVALID_ID) :
			RenderableObjectHandle(handleID)
		{}

		static DeviceMaterialHandle	Null() { return DeviceMaterialHandle(); }

		[[nodiscard]] std::pair<uint32_t, uint32_t>	DecomposeMaterialAndShaderIndices() const
		{
			return { (uint32_t) (Get() >> 32), (uint32_t)Get() };
		}

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



	};


}
