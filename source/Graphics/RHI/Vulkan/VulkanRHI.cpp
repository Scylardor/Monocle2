#include "VulkanRHI.h"

#ifdef MOE_VULKAN


moe::VulkanRHI::VulkanRHI(const RequiredExtensionList& extensionsList, std::string_view appName, std::string_view engineName)
{
	// Create the Vulkan Instance
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = appName.data();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = engineName.data();
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_2;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = extensionsList.ExtensionNumber;
	createInfo.ppEnabledExtensionNames = extensionsList.ExtensionList;
}


moe::VulkanRHI::~VulkanRHI()
{
	// Destroy the Vulkan Instance
}


#endif // MOE_VULKAN
