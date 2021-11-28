#pragma once
#include "Core/Resource/Material/MaterialResource.h"

namespace moe
{
	/*
	 * An abstract class for pipeline state object (PSO) management.
	 */
	class IPipelineManager
	{
	public:

		virtual ~IPipelineManager() = default;

		virtual bool	CreatePipelineStateObjectLayout(PipelineDescription const& pipeDesc) = 0;

	};


}
