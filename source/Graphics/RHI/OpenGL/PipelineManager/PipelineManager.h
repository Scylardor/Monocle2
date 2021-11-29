#pragma once
#include <Monocle_Graphics_Export.h>

#include "Core/Resource/Material/MaterialResource.h"
#include "Graphics/RHI/PipelineManager/PipelineManager.h"

namespace moe
{
	/*
	 * An abstract class for pipeline state object (PSO) management.
	 */
	class OpenGL4PipelineManager : public IPipelineManager
	{
	public:

		~OpenGL4PipelineManager() override = default;

		Monocle_Graphics_API bool	CreatePipelineStateObjectLayout(PipelineDescription const& pipeDesc) override;

	};


}
