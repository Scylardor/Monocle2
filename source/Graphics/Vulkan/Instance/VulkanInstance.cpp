
#ifdef MOE_VULKAN

#include "VulkanInstance.h"
#include "Graphics/Vulkan/ValidationLayers/VulkanValidationLayers.h"
#include "Graphics/Vulkan/Device/VulkanDevice.h"
namespace moe
{



	bool VulkanInstance::Initialize(CreationParams&& instanceParams)
	{
		
		m_creationParams = std::move(instanceParams);

		// Activating the dynamic dispatcher in order to easily use Vulkan EXTension functions.
		// We need to initialize it a first time in order to enable validation layers. see https://github.com/KhronosGroup/Vulkan-Hpp#extensions--per-device-function-pointers
		InitDynamicDispatcherFirstStep();

		// Enable validation layers if we're in Debug mode
		if constexpr (VulkanValidationLayers::AreEnabled())
		{
			bool validationLayersEnabled = EnableValidationLayersSupport();
			MOE_DEBUG_ASSERT(validationLayersEnabled);
		}

		bool ok = CheckRequiredExtensionsAvailability(m_creationParams.RequiredExtensions);
		if (ok)
			ok = Create();

		if (!ok)
		{
			MOE_ERROR(moe::ChanGraphics, "An error occurred during Vulkan Instance creation.");
		}

		// Next, we need to further initialize the dynamic dispatcher in order to create the debug messenger.
		InitDynamicDispatcherSecondStep();

		if constexpr (VulkanValidationLayers::AreEnabled())
		{
			SetupDebugMessenger();
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

		// Using a special debug messenger to catch potential errors during instance creation.
		// keeping it out of the if so it survives until the instance creation.
		vk::DebugUtilsMessengerCreateInfoEXT instanceCreationDebugMessengerInfo;

		if constexpr (VulkanValidationLayers::AreEnabled())
		{
			createInfo.enabledLayerCount = (uint32_t)VulkanValidationLayers::Names().size();
			createInfo.ppEnabledLayerNames = VulkanValidationLayers::Names().data();

			instanceCreationDebugMessengerInfo = VulkanDebugMessenger::GenerateCreateInfo();
			createInfo.pNext = &instanceCreationDebugMessengerInfo;
		}

		// Create the Vulkan Instance!
		try
		{
			m_instance = vk::createInstanceUnique(createInfo, nullptr);
			MOE_DEBUG_ASSERT(m_instance.get());
		}
		catch (const vk::SystemError & err)
		{
			MOE_ERROR(moe::ChanGraphics, "Error : could not create Vulkan Instance (code : %i, message: %s), what: %s", err.code(), err.code().message(), err.what());
			// TODO : throw custom exception
			throw std::runtime_error("Monocle failed to create Vulkan instance!");
		}

		return true;
	}


	void VulkanInstance::InitDynamicDispatcherFirstStep()
	{
		vk::DynamicLoader dl;
		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
		VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
	}


	void VulkanInstance::InitDynamicDispatcherSecondStep()
	{
		VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance.get());
	}

	void VulkanInstance::InitDynamicDispatcherThirdStep(const MyVkDevice& device)
	{
		VULKAN_HPP_DEFAULT_DISPATCHER.init((*device));
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

			for (auto iReqExt = 0u; iReqExt < tmpExtList.Count(); iReqExt++)
			{
				if (strcmp(prop.extensionName, tmpExtList.List()[iReqExt]) == 0)
				{
					// found this one
					tmpExtList.RemoveExtensionAt(iReqExt);
					break;
				}
			}

			if (tmpExtList.Count() == 0)
				break;
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


	bool VulkanInstance::CheckValidationLayersSupport()
	{
		for (const auto& validationLayer : VulkanValidationLayers::Names())
		{
			bool layerFound = false;

			for (const auto& prop : m_layerProperties)
			{
				if (strcmp(prop.layerName, validationLayer) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				MOE_ERROR(moe::ChanGraphics, "Missing validation layer %s!", validationLayer);
				MOE_DEBUG_ASSERT(false);
				return false;
			}
		}

		return true;
	}


	bool VulkanInstance::EnableValidationLayersSupport()
	{
		// Are validation layer supported ? If yes, enable the VK debug utils extension.

		m_layerProperties = vk::enumerateInstanceLayerProperties();

		bool allLayersSupported = CheckValidationLayersSupport();

		if (allLayersSupported)
		{
			//  We don't really need to check for the existence of this extension, because it should be implied by the availability of the validation layers.
			m_creationParams.RequiredExtensions.AddExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return allLayersSupported;
	}


	void VulkanInstance::SetupDebugMessenger()
	{
		m_debugMessenger.Create(*this);
	}
}

#endif // MOE_VULKAN

