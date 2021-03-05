#pragma once

#ifdef MOE_VULKAN

#include <vulkan/vulkan.hpp>


namespace moe
{

	struct RequiredExtensionList
	{
	public:
		RequiredExtensionList() = default;

		RequiredExtensionList(uint32_t extensionNbr, const char** extensionList) :
			ExtensionNumber(extensionNbr), ExtensionList(extensionList)
		{}

		uint32_t		ExtensionNumber = 0;
		const char**	ExtensionList = nullptr;

	};


	class VulkanRHI
	{
	public:

		VulkanRHI(const RequiredExtensionList& extensionsList, std::string_view appName = "Unnamed Application", std::string_view engineName = "No Engine");
		~VulkanRHI();

	private:

		VkInstance instance;




	};



}

#endif