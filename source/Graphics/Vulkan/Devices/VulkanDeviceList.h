#pragma once

#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

#include <optional>

namespace moe
{
	// TODO: Should be renamed VulkanDevice once the placeholder one is gone
	class MyVkDevice
	{
	public:

		struct SwapchainSupportParameters
		{
			VkSurfaceCapabilitiesKHR			SurfaceCapabilities;
			std::vector<vk::SurfaceFormatKHR>	AvailableSurfaceFormats;
			std::vector<vk::PresentModeKHR>		AvailablePresentModes;
		};



		MyVkDevice(vk::PhysicalDevice&& physDev);

		const auto&	Properties() const
		{ return m_properties; }

		const auto& Features() const
		{
			return m_features;
		}

		const auto&	GetSwapchainSupportParameters() const
		{
			return m_swapchainSupportParams;
		}


		uint32_t	GetScoreForPresentSurface(vk::SurfaceKHR presentSurface) ;

		bool		FetchQueueFamilies(vk::SurfaceKHR presentSurface);
		bool		FetchExtensionProperties();
		bool		HasGraphicsAndPresentOnSameQueue() const;

		bool		FetchSwapchainSupportParameters(vk::SurfaceKHR presentSurface);


	private:
		using FamilyIndex = std::optional<uint32_t>;
		struct FamilyIndices
		{
			FamilyIndex	GraphicsFamilyIdx;
			FamilyIndex	PresentFamilyIdx;

			bool	IsComplete() const
			{
				return GraphicsFamilyIdx.has_value() && PresentFamilyIdx.has_value();
			}
		};

		bool		HasRequiredGraphicsQueueFamilies() const;
		bool		FetchGraphicsFeatures() ;
		void		FetchGraphicsProperties();
		uint32_t	ComputeGraphicsScore();

		// No Unique handle for Physical Device because it doesn't need to be destroyed
		vk::PhysicalDevice	m_physicalDevice; // PhysicalDevice doesn't actually need to be destroyed, so no Unique handle.
		vk::UniqueDevice	m_logicalDevice;

		vk::PhysicalDeviceProperties			m_properties;
		std::vector <vk::ExtensionProperties>	m_extensionProperties;
		vk::PhysicalDeviceFeatures				m_features;
		std::vector<vk::QueueFamilyProperties>	m_queueFamilyProps;

		SwapchainSupportParameters				m_swapchainSupportParams;


		FamilyIndices	m_queueIndices;
	};


	/**
	 * @brief A class meant to represent the available devices detected by our instance.
	 */
	class VulkanDeviceList
	{
	public:

		VulkanDeviceList() = default;

		bool	Initialize(const vk::Instance& instance);

		void	PopulateDevices(const vk::Instance& instance);

		MyVkDevice* PickGraphicsDevice(vk::SurfaceKHR presentSurface);

	private:


		std::vector<MyVkDevice>	m_devices;
		MyVkDevice*				m_graphicsDevice = nullptr;


	};


}

#endif // MOE_VULKAN

