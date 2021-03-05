#ifdef MOE_VULKAN

#include "VulkanInstance.h"

namespace moe
{
#ifdef MOE_DEBUG
	const bool	S_enableValidationLayers = true;
#else
	const bool	S_enableValidationLayers = false;
#endif




	bool VulkanInstance::Initialize(VulkanInstance::CreationParams&& instanceParams)
	{
		m_creationParams = std::move(instanceParams);

		// Enable validation layers if we're in Debug mode
		if (S_enableValidationLayers)
			m_creationParams.RequiredExtensions.AddExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		bool ok = CheckRequiredExtensionsAvailability(m_creationParams.RequiredExtensions);
		if (ok)
			ok = Create();

		if (!ok)
		{
			MOE_ERROR(moe::ChanGraphics, "An error occurred during Vulkan Instance creation.");
		}

		return ok;
	}

	bool VulkanInstance::Create()
	{
		auto appInfo = vk::ApplicationInfo(
			m_creationParams.appName.data(),
			VK_MAKE_VERSION(1, 0, 0),
			m_creationParams.engineName.data(),
			VK_MAKE_VERSION(1, 0, 0),
			VK_API_VERSION_1_2
		);

		auto createInfo = vk::InstanceCreateInfo(
			vk::InstanceCreateFlags(),
			&appInfo,
			0, nullptr, // enabled layers
			(uint32_t)(m_creationParams.RequiredExtensions.Count()), m_creationParams.RequiredExtensions.List() // enabled extensions
		);

		if constexpr (S_enableValidationLayers)
		{
			createInfo.enabledLayerCount = (uint32_t)(S_USED_VALIDATION_LAYERS.size());
			createInfo.ppEnabledLayerNames = S_USED_VALIDATION_LAYERS.data();
		}

		// Create the Vulkan Instance!
		try
		{
			m_instance = vk::createInstanceUnique(createInfo, nullptr);
		}
		catch (const vk::SystemError & err)
		{
			MOE_ERROR(moe::ChanGraphics, "Error : could not create Vulkan Instance (code : %i, message: %s), what: %s", err.code(), err.code().message(), err.what());
			// TODO : throw custom exception
			throw std::runtime_error("Monocle failed to create Vulkan instance!");
		}

		return true;
	}


	void VulkanInstance::RetrieveExtensionProperties()
	{
		m_extensionProperties = vk::enumerateInstanceExtensionProperties();
		MOE_INFO(moe::ChanGraphics, "Retrieving Vulkan Instance Extension Properties... Found %lu extensions.", m_extensionProperties.size());
	}


	bool VulkanInstance::CheckRequiredExtensionsAvailability(const ExtensionList& requiredExtensions)
	{
		RetrieveExtensionProperties();

		// make a copy because we're going to pop until everything is found (dont want to lose data)
		ExtensionList tmpExtList = requiredExtensions;

		for (const auto& prop : m_extensionProperties)
		{
			MOE_INFO(moe::ChanGraphics, "\t%s", prop.extensionName.data());

			if (tmpExtList.Count() != 0)
			{
				for (auto iReqExt = 0u; iReqExt < tmpExtList.Count(); iReqExt++)
				{
					if (strcmp(prop.extensionName, tmpExtList.List()[iReqExt]) == 0)
					{
						// found this one
						tmpExtList.RemoveExtensionAt(iReqExt);
						break;
					}
				}
			}
		}

		bool allRequirementsMet = (tmpExtList.Count() == 0);
		if (allRequirementsMet)
		{
			MOE_INFO(moe::ChanGraphics, "All required instance extensions were found!");
		}
		else
		{
			MOE_ERROR(moe::ChanGraphics, "Required Vulkan extensions are missing!");
			MOE_DEBUG_ASSERT(false);
		}

		return allRequirementsMet;
	}
}

#endif // MOE_VULKAN

