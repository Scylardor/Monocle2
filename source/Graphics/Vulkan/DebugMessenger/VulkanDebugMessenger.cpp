#ifdef MOE_VULKAN

#include "Graphics/Vulkan/Instance/VulkanInstance.h"

// Only activate verbose and info in Debug builds
#ifdef MOE_DEBUG
const bool S_wantVerboseAndInfo = true;
#else
const bool S_wantVerboseAndInfo = false;
#endif


void moe::VulkanDebugMessenger::Create(VulkanInstance& myInstance)
{
	auto createInfo = GenerateCreateInfo();
	const vk::Instance& inst = myInstance.Instance();

	m_messenger = inst.createDebugUtilsMessengerEXTUnique(createInfo, nullptr);
	MOE_DEBUG_ASSERT(m_messenger.get());
}


vk::DebugUtilsMessengerCreateInfoEXT moe::VulkanDebugMessenger::GenerateCreateInfo()
{
	vk::DebugUtilsMessengerCreateInfoEXT messengerInfo;

	auto severity =
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
	if (S_wantVerboseAndInfo)
	{
		severity |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;
	}

	messengerInfo.setMessageSeverity(severity);

	messengerInfo.setMessageType(
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);

	messengerInfo.setPfnUserCallback(&MessengerCallback);
	messengerInfo.setPUserData(nullptr); // maybe for later usage

	return messengerInfo;
}


VkBool32 moe::VulkanDebugMessenger::MessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void*)
{


	if (messageSeverity <= (int)vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
	{
		MOE_INFO(moe::ChanGraphics, "Monocle Vulkan validation layer diagnostic: [%s] - %s",
			std::to_string(messageType),
			pCallbackData->pMessage);
	}
	else if (messageSeverity == (int)vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
	{
		MOE_WARNING(moe::ChanGraphics, "Monocle Vulkan validation layer warning: [%s] - %s",
			std::to_string(messageType),
			pCallbackData->pMessage);
	}
	else
	{
		MOE_ERROR(moe::ChanGraphics, "Monocle Vulkan validation layer error: [%s] - %s",
			std::to_string(messageType),
			pCallbackData->pMessage);
	}

	// The callback returns a boolean that indicates if the Vulkan call that triggered the validation layer message should be aborted.
	// If the callback returns true, then the call is aborted with the VK_ERROR_VALIDATION_FAILED_EXT error.
	// This is normally only used to test the validation layers themselves, so you should always return VK_FALSE.
	return VK_FALSE;
}


#endif // MOE_VULKAN
