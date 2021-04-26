#ifdef MOE_VULKAN

#include "FramebufferFactory.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"
#include "Graphics/Vulkan/Swapchain/VulkanSwapchain.h"

namespace moe
{
	bool FramebufferFactory::Initialize(const MyVkDevice& device, const VulkanSwapchain& swapChain)
	{
		MOE_ASSERT(m_device == nullptr && m_swapChain == nullptr);
		m_device = &device;
		m_swapChain = &swapChain;

		return true;
	}


	FramebufferFactory::FramebufferID FramebufferFactory::CreateFramebuffer(vk::FramebufferCreateInfo& fbInfo)
	{
		// Scan the framebuffer create info for any of the "magic" image views of the swapchain,
		// then adjust them if necessary.
		// If it needs patching I need to allocate another array because of the pAttachments array is const... cannot change them in-place

		std::optional<int> swapchainColorIdx{};
		std::optional<int> swapchainDepthIdx{};

		// this assumes each swap chain target only appears once in the framebuffer info. Or it's probably wrong.
		for (auto iAttach = 0u; iAttach < fbInfo.attachmentCount; iAttach++)
		{
			if (fbInfo.pAttachments[iAttach] == SWAPCHAIN_COLOR_ATTACHMENT_VIEW)
			{
				swapchainColorIdx = iAttach;
			}
			else if (fbInfo.pAttachments[iAttach] == SWAPCHAIN_DEPTH_STENCIL_ATTACHMENT_VIEW)
			{
				swapchainDepthIdx = iAttach;
			}
		}


		FramebufferID newFbID;
		if (false == (swapchainColorIdx.has_value() || swapchainDepthIdx.has_value()))
		{
			// just create it as is
			newFbID = CreateNewFramebuffer(fbInfo);

		}
		else // it's a bit more involved : we need N new framebuffers with N = swap chain images count
		{
			newFbID = CreateSwapchainAttachedFramebuffer(fbInfo, swapchainColorIdx, swapchainDepthIdx);
		}

		return newFbID;
	}


	vk::Framebuffer FramebufferFactory::GetFramebuffer(FramebufferID fbID) const
	{
		int fbIndex = fbID;
		if (IsSwapchainAttachedFramebuffer(fbID))
		{
			// offset the framebuffer ID with the index of the swap chain image currently in flight
			fbIndex += m_swapChain->GetImageInFlightIndex();
		}

		return m_framebuffers[fbIndex].get();
	}


	FramebufferFactory::FramebufferID FramebufferFactory::CreateNewFramebuffer(const vk::FramebufferCreateInfo& fbInfo)
	{
		FramebufferID newFbID = GetNewFramebufferID();
		m_framebuffers.push_back(Device()->createFramebufferUnique(fbInfo));
		return newFbID;
	}


	FramebufferFactory::FramebufferID FramebufferFactory::CreateSwapchainAttachedFramebuffer(vk::FramebufferCreateInfo& fbInfo,
		std::optional<int> swapchainColorIdx, std::optional<int> swapchainDepthIdx)
	{
		std::vector<vk::ImageView> patchedAttachmentsList;
		patchedAttachmentsList.reserve(fbInfo.attachmentCount);
		patchedAttachmentsList.assign(fbInfo.pAttachments, fbInfo.pAttachments + fbInfo.attachmentCount);

		FramebufferID newFbID = GetNewFramebufferID();

		// If this framebuffer references swap chain attachments, enforce that its dimensions match the swapchain's.
		VkExtent2D swapchainExtent = m_swapChain->GetSwapchainImageExtent();
		fbInfo.width = swapchainExtent.width;
		fbInfo.height = swapchainExtent.height;

		// we need N new framebuffers with N = swap chain images count
		m_framebuffers.reserve(m_framebuffers.size() + m_swapChain->GetImagesInFlightCount());
		for (auto iSCImg = 0u; iSCImg < m_swapChain->GetImagesInFlightCount(); iSCImg++)
		{
			if (swapchainColorIdx.has_value())
			{
				patchedAttachmentsList[*swapchainColorIdx] = m_swapChain->GetColorAttachmentView(iSCImg);
			}
			if (swapchainDepthIdx.has_value())
			{
				patchedAttachmentsList[*swapchainDepthIdx] = m_swapChain->GetDepthAttachmentView();
			}

			fbInfo.pAttachments = patchedAttachmentsList.data(); // the actual patchup
			CreateNewFramebuffer(fbInfo);
		}

		// write the informations about this framebuffer's swapchain attachments for when we recreate the swapchain.
		std::vector<SwapchainFramebufferAttachment>	swapchainAttachmentsInfo;
		swapchainAttachmentsInfo.reserve(2); // color + depth
		if (swapchainColorIdx.has_value())
		{
			swapchainAttachmentsInfo.emplace_back(*swapchainColorIdx, SWAPCHAIN_COLOR_ATTACHMENT_VIEW);
		}
		if (swapchainDepthIdx.has_value())
		{
			swapchainAttachmentsInfo.emplace_back(*swapchainDepthIdx, SWAPCHAIN_DEPTH_STENCIL_ATTACHMENT_VIEW);
		}

		// Add this framebuffer to the lookup map of "swapchain attached framebuffers"
		m_framebuffersWithSwapchainAttachments.emplace(newFbID, std::move(swapchainAttachmentsInfo));

		return newFbID;
	}
}


#endif // MOE_VULKAN
