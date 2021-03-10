#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

namespace moe
{
	class MyVkDevice;

	class SwapchainFrame
	{
	public:
		struct VulkanSwapchainImage
		{
			// not a UniqueImage? This isn't a mistake!
			// because those images are retrieved via getSwapchainImagesKHR, they get automatically freed by the UniqueSwapchain handle dtor!
			vk::Image			Handle;
			vk::UniqueImageView	View;
		};


		SwapchainFrame() = default;

		bool	Initialize(const MyVkDevice& device, vk::Image frameImage, vk::Format swapChainImageFormat);


		VulkanSwapchainImage	FrameImage{};

	private:


		vk::UniqueSemaphore		m_presentCompleteSemaphore{};
		vk::UniqueSemaphore		m_renderCompleteSemaphore{};

	};



	class SwapchainFrameList
	{
	public:

		bool	Initialize(const MyVkDevice& device, vk::SwapchainKHR swapChain, vk::Format swapChainImageFormat);
		void	Teardown(const MyVkDevice& device);

		void	ClearImages(vk::Device device);

	private:

		std::vector<SwapchainFrame>	m_swapChainImages;
		uint32_t					m_currentFrameIdx = 0;

	};

}


#endif // MOE_VULKAN