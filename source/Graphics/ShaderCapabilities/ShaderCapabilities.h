#pragma once
#include <cstdint>

#include "Math/Matrix.h"


namespace moe
{
	class RenderObject;
	class RenderScene;
	struct MaterialPassDescription;

	enum class ReservedCapacitySets : uint8_t
	{
		OBJECT_TRANSFORMS	= 0,
		VIEW_MATRICES		= 1,
	};



	struct SC_ObjectTransform
	{
		static void OnAdded(MaterialPassDescription& shaderPass);
		static void	OnInitialized(RenderScene*, RenderObject&);
		static void	OnRemoved(RenderScene*, RenderObject&);
	};


	struct SC_ViewMatrices
	{
		static void OnAdded(MaterialPassDescription& shaderPass);
		static void	OnInitialized(RenderScene*, RenderObject&);
		static void	OnRemoved(RenderScene*, RenderObject&);
	};
}
