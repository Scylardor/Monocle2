#ifdef MOE_VULKAN

#include "RenderScene.h"

#include "Graphics/Vulkan/Buffer/VulkanBuffer.h"

#include "Core/Containers/ObjectPool/ObjectPool.h"

namespace moe
{

	void RenderScene::Initialize(uint32_t nbrActors)
	{
		m_objects.Reserve(nbrActors);
	}
}


#endif // MOE_VULKAN
