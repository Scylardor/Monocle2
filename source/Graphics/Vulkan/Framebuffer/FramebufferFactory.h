#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

#include <optional>

namespace moe
{
	class MyVkDevice;
	class VulkanSwapchain;

	class FramebufferFactory
	{
	public:

		typedef	uint32_t	FramebufferID;

		// Keeps track of the swapchain attachments in the framebuffer. Includes:
		// - which index an attachment was at
		// - the "magic" attachment view ID to know which attachment that was
		struct SwapchainFramebufferAttachment
		{
			SwapchainFramebufferAttachment(int index, vk::ImageView view) :
				AttachementIndex(index), AttachmentView(view)
			{}

			int				AttachementIndex{};
			vk::ImageView	AttachmentView{};
		};

		typedef std::unordered_map<FramebufferID, std::vector<SwapchainFramebufferAttachment>>	SwapchainAttachmentsLookup;


		bool	Initialize(const MyVkDevice& device, const VulkanSwapchain& swapChain);

		FramebufferID	CreateFramebuffer(vk::FramebufferCreateInfo& fbInfo);

		vk::Framebuffer	GetFramebuffer(FramebufferID fbID) const;



		inline static const vk::ImageView SWAPCHAIN_COLOR_ATTACHMENT_VIEW{ (VkImageView)0 };

		inline static const vk::ImageView SWAPCHAIN_DEPTH_STENCIL_ATTACHMENT_VIEW{ (VkImageView)1 };

		// Will be automatically replaced when creating a framebuffer with a swapchain attachment in it.
		inline static const uint32_t SWAPCHAIN_FRAMEBUFFER_WIDTH = 0;
		inline static const uint32_t SWAPCHAIN_FRAMEBUFFER_HEIGHT = SWAPCHAIN_FRAMEBUFFER_WIDTH;


	protected:

	private:

		const MyVkDevice*		m_device = nullptr;

		const MyVkDevice& Device() const
		{
			MOE_ASSERT(m_device != nullptr);
			return *m_device;
		}

		const VulkanSwapchain*	m_swapChain = nullptr;

		FramebufferID	GetNewFramebufferID() const
		{
			return  (uint32_t)m_framebuffers.size();
		}

		bool	IsSwapchainAttachedFramebuffer(FramebufferID fbID) const
		{
			return (m_framebuffersWithSwapchainAttachments.find(fbID) != m_framebuffersWithSwapchainAttachments.end());
		}


		FramebufferID	CreateNewFramebuffer(const vk::FramebufferCreateInfo& fbInfo);

		FramebufferID	CreateSwapchainAttachedFramebuffer(vk::FramebufferCreateInfo& fbInfo, std::optional<int> swapchainColorIdx, std::optional<int> swapchainDepthIdx);


		// framebuffers with swapchain attachments in them. Those will need to be recreated upon swap chain recreation
		SwapchainAttachmentsLookup			m_framebuffersWithSwapchainAttachments;

		// TODO: Technically should be an object pool...
		std::vector<vk::UniqueFramebuffer>	m_framebuffers;


	};
}


#endif // MOE_VULKAN
