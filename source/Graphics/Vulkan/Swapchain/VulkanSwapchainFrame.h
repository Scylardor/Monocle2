#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

namespace moe
{
	class MyVkDevice;

	struct VulkanSwapchainImage
	{
		VulkanSwapchainImage(const MyVkDevice& device, vk::Image swapchainImage, vk::Format imageFormat);

		void	AcquireFence(vk::Device device, vk::Fence newInFlightFence);

		// not a UniqueImage? This isn't a mistake!
		// because those images are retrieved via getSwapchainImagesKHR, they get automatically freed by the UniqueSwapchain handle dtor!
		vk::Image			Image;
		vk::UniqueImageView	View;
		vk::Fence			InFlightFence{}; // a reference to a fence in one of the swapchain frames
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