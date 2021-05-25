#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"
#include "VulkanSwapchainTexture.h"

namespace moe
{
	class MyVkDevice;

	struct VulkanSwapchainImage
	{
		VulkanSwapchainImage(MyVkDevice& device, vk::Image swapchainImage, const VulkanTextureBuilder & swapchainTextureBuilder);

		VulkanSwapchainImage(VulkanSwapchainImage&& other)
		{
			*this = std::move(other);
		}

		VulkanSwapchainImage& operator=(VulkanSwapchainImage&& other)
		{
			if (this != &other)
			{
				Texture = std::move(other.Texture);
				InFlightFence = other.InFlightFence;
			}

			return *this;
		}


		void	AcquireFence(vk::Device device, vk::Fence newInFlightFence);

		void	Free(MyVkDevice& device);

		VulkanSwapchainTexture	Texture;
		vk::Fence				InFlightFence{}; // a reference to a fence in one of the swapchain frames
	};

	class VulkanSwapchainFrame
	{
	public:

		VulkanSwapchainFrame(const MyVkDevice& device);

		bool	WaitForSubmitFence(const MyVkDevice& device);

		void	Free(const MyVkDevice& device);

		vk::Semaphore		PresentCompleteSemaphore{};
		vk::Semaphore		RenderCompleteSemaphore{};
		vk::Fence			QueueSubmitFence{};

	private:


	};



}


#endif // MOE_VULKAN