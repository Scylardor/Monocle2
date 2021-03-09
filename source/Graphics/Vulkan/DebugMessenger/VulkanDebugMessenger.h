
#ifdef MOE_VULKAN
#pragma once

#include "Graphics/Vulkan/VulkanMacros.h"

namespace moe
{
	class VulkanDebugMessenger
	{
		public:

			void	Create(class VulkanInstance& myInstance);

			static vk::DebugUtilsMessengerCreateInfoEXT	GenerateCreateInfo();

			static VKAPI_ATTR VkBool32 VKAPI_CALL	MessengerCallback(
				VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

		private:

			vk::UniqueDebugUtilsMessengerEXT	m_messenger;
	};


}

#endif // MOE_VULKAN
