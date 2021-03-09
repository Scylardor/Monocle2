#pragma once

#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

#include "Graphics/Vulkan/DebugMessenger/VulkanDebugMessenger.h"


#include <string>

namespace moe
{
	class VulkanInstance
	{
	public:

		struct ExtensionList
		{
		public:
			ExtensionList() = default;

			ExtensionList(uint32_t extensionNbr, const char** extensionList) :
				ExtensionNumber(extensionNbr), Extensions(extensionList, extensionList + extensionNbr)
			{}

			void	AddExtension(std::string_view extension)
			{
				Extensions.emplace_back(extension.data());
				ExtensionNumber++;
			}

			void	RemoveExtensionAt(size_t idx)
			{
				// erase with last swap trick
				std::iter_swap(Extensions.begin() + idx, Extensions.end() - 1);
				Extensions.pop_back();
				ExtensionNumber--;
			}

			const char* const * List() const
			{ return Extensions.data();  }

			auto	Count() const
			{
				return ExtensionNumber;
			}


		private:

			uint32_t					ExtensionNumber = 0; // TODO: probably became useless, to remove
			std::vector<const char*>	Extensions;

		};

		struct CreationParams
		{
			ExtensionList	RequiredExtensions;
			std::string		appName = "Unnamed Application";
			std::string		engineName = "No Engine";
		};


		VulkanInstance() = default;

		bool	Initialize(CreationParams&& instanceParams);

		const vk::Instance& Instance() const
		{ return m_instance.get(); }


	private:

		bool	Create();

		void	InitDynamicDispatcherFirstStep();
		void	InitDynamicDispatcherSecondStep();

	public:
		// This one is public because it needs to be called from outside.
		void	InitDynamicDispatcherThirdStep(const class MyVkDevice& device);

	private:

		void	RetrieveExtensionProperties();
		bool	CheckRequiredExtensionsAvailability(const ExtensionList& requiredExtensions);

		bool	CheckValidationLayersSupport();
		bool	EnableValidationLayersSupport();

		void	SetupDebugMessenger();

		vk::UniqueInstance		m_instance;

		VulkanDebugMessenger	m_debugMessenger;

		std::vector<vk::ExtensionProperties>	m_extensionProperties;
		std::vector<vk::LayerProperties>		m_layerProperties;

		CreationParams		m_creationParams;

	};


}

#endif // MOE_VULKAN
