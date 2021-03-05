#ifdef MOE_VULKAN

#pragma once

#include <vulkan/vulkan.hpp>

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

		bool	Initialize(VulkanInstance::CreationParams&& instanceParams);


		const vk::Instance& Instance() const
		{ return m_instance.get(); }

	protected:

	private:
		bool	Create();

		void	RetrieveExtensionProperties();
		bool	CheckRequiredExtensionsAvailability(const ExtensionList& requiredExtensions);

		vk::UniqueInstance	m_instance;

		std::vector<vk::ExtensionProperties>	m_extensionProperties;

		CreationParams		m_creationParams;


		static inline const std::array<const char*, 1>	S_USED_VALIDATION_LAYERS =
		{ "VK_LAYER_KHRONOS_validation" };

	};
}

#endif // MOE_VULKAN
