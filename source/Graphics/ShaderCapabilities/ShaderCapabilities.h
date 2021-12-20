#pragma once
#include <cstdint>

#include "Math/Matrix.h"


namespace moe
{
	class RenderObject;
	class RenderScene;
	struct MaterialPassDescription;

	enum class ReservedCapacitySets
	{
		OBJECT_TRANSFORMS = 0
	};



	struct SC_ObjectTransform
	{
		static void OnAdded(MaterialPassDescription& shaderPass);
		static void	OnInitialized(RenderScene*, RenderObject&);
		static void	OnRemoved(RenderScene*, RenderObject&);
	};
}
