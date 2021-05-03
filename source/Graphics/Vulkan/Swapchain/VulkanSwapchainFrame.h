#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"
#include "VulkanSwapchainTexture.h"

namespace moe
{
	class MyVkDevice;

	struct VulkanSwapchainImage
	{
		VulkanSwapchainImage(const MyVkDevice& device, vk::Image swapchainImage, const VulkanTextureBuilder & swapchainTextureBuilder);

		void	AcquireFence(vk::Device device, vk::Fence newInFlightFence);

		VulkanSwapchainTexture	Texture;
		vk::Fence				InFlightFence{}; // a reference to a fence in one of the swapchain frames
	};

	class VulkanSwapchainFrame
	{
	public:

		VulkanSwapchainFrame(const MyVkDevice& device);

		bool	WaitForSubmitFence(const MyVkDevice& device);

		vk::UniqueSemaphore		PresentCompleteSemaphore{};
		vk::UniqueSemaphore		RenderCompleteSemaphore{};
		vk::UniqueFence			QueueSubmitFence{};

	private:


	};



}


#endif // MOE_VULKAN