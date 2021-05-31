#ifdef MOE_VULKAN

#include "RenderScene.h"

#include "Graphics/Vulkan/Buffer/VulkanBuffer.h"

#include "Core/Containers/ObjectPool/ObjectPool.h"

namespace moe
{

	void RenderScene::Initialize(MyVkDevice& device, uint32_t maxFrameCount, uint32_t nbrActors)
	{
		m_objects.Reserve(nbrActors);

		m_cameras.Initialize(device, maxFrameCount);
	}
}


#endif // MOE_VULKAN
