#ifdef MOE_VULKAN

#include "RenderScene.h"


namespace moe
{

	void VulkanRenderScene::Initialize(MyVkDevice& device, const VulkanSwapchain& swapchain, uint32_t maxFrameCount, uint32_t nbrActors)
	{
		m_objects.Reserve(nbrActors);

		m_cameras.Initialize(device, swapchain, maxFrameCount);
	}


	void VulkanRenderScene::Update(uint32_t frameIndex)
	{
		m_cameras.Update(frameIndex);
	}
}


#endif // MOE_VULKAN
