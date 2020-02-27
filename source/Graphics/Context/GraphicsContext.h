#pragma once

#include "Core/Preprocessor/moeDLLVisibility.h"

namespace moe
{
	class GraphicsContextDescriptor;

	/*
		A pure virtual interface describing a graphics context.
		Necessary to create a graphics context bound to a window in a Monocle application.
		It is created following the description of a GraphicsContextDescriptor.
	*/
	class MOE_DLL_API IGraphicsContext
	{
	public:
		typedef	void* GenericWindowHandle; // This type has to be statically cast by the graphics context to the actual window type it expects.

		virtual ~IGraphicsContext() {}

		virtual void	InitPreWindowCreation(const GraphicsContextDescriptor& contextDesc) = 0;

		/* InitPostWindowCreation returns whether it succeeded or not. */
		virtual bool	InitPostWindowCreation(const GraphicsContextDescriptor& contextDesc, GenericWindowHandle windowHandle) = 0;
	};

}