#ifdef MOE_VULKAN

#include "VulkanDeviceList.h"

namespace moe
{
	MyVkDevice::MyVkDevice(vk::PhysicalDevice&& physDev) :
		m_physicalDevice(std::move(physDev))
	{
	}


	uint32_t MyVkDevice::GetScoreForPresentSurface(vk::SurfaceKHR presentSurface)
	{
		// Fetching queue families is needed to figure out if both graphics and present queues are available.
		// Fetching swap chain support parameters for this surface can also be done as soon as here.
		if (false == (FetchGraphicsFeatures()
			&& FetchExtensionProperties()
			&& FetchQueueFamilies(presentSurface)
			&& FetchSwapchainSupportParameters(presentSurface)))
			return 0;

		FetchGraphicsProperties();

		uint32_t score = ComputeGraphicsScore();
		return score;
	}


	bool MyVkDevice::FetchQueueFamilies(vk::SurfaceKHR presentSurface)
	{
		m_queueFamilyProps = m_physicalDevice.getQueueFamilyProperties();

		for (auto iFam = 0u; iFam < m_queueFamilyProps.size(); iFam++)
		{
			if (m_queueFamilyProps[iFam].queueFlags & vk::QueueFlagBits::eGraphics)
			{
				m_queueIndices.GraphicsFamilyIdx = iFam;
			}

			// Check for support for the present surface because we want to display images to it
			VkBool32 presentSupported = false;
			MOE_VK_CHECK(m_physicalDevice.getSurfaceSupportKHR(iFam, presentSurface, &presentSupported));
			if (presentSupported)
			{
				m_queueIndices.PresentFamilyIdx = iFam;
			}

			if (m_queueIndices.IsComplete())
				break;
		}

		bool hasAllIndices = m_queueIndices.IsComplete();
		return hasAllIndices;
	}


	bool MyVkDevice::FetchExtensionProperties()
	{
		m_extensionProperties = m_physicalDevice.enumerateDeviceExtensionProperties();

		// So far, the only extension we have to manage is swap chain.
		std::array<const char*, 1> neededExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		// TODO: this could be done a bit better I think ?
		for (auto& ext : neededExtensions)
		{
			auto found = std::find_if(
				m_extensionProperties.begin(),
				m_extensionProperties.end(),
				[ext](auto& extProp) { return (strcmp(ext, extProp.extensionName) == 0); });

			if (found == m_extensionProperties.end())
				return false; // One needed extension was not found
		}

		return true;
	}


	bool MyVkDevice::HasGraphicsAndPresentOnSameQueue() const
	{
		return m_queueIndices.IsComplete()
		&& m_queueIndices.GraphicsFamilyIdx == m_queueIndices.PresentFamilyIdx;
	}


	bool MyVkDevice::FetchSwapchainSupportParameters(vk::SurfaceKHR presentSurface)
	{
		m_swapchainSupportParams.SurfaceCapabilities = m_physicalDevice.getSurfaceCapabilitiesKHR(presentSurface);
		m_swapchainSupportParams.AvailableSurfaceFormats = m_physicalDevice.getSurfaceFormatsKHR(presentSurface);
		m_swapchainSupportParams.AvailablePresentModes = m_physicalDevice.getSurfacePresentModesKHR(presentSurface);

		return (!m_swapchainSupportParams.AvailableSurfaceFormats.empty()
			&& !m_swapchainSupportParams.AvailablePresentModes.empty());
	}


	bool MyVkDevice::FetchGraphicsFeatures()
	{
		m_features = m_physicalDevice.getFeatures();

		// List of features the engine cannot live without
		// TODO: Right now this is arbitrarily set by the engine and may be made customizable later on...
		// e.g. you may not care about multi viewport capabilities.
		bool hasAllRequiredFeatures = (
			m_features.geometryShader
			&& m_features.tessellationShader
			&& m_features.multiDrawIndirect
			&& m_features.multiViewport
			&& m_features.depthClamp
			&& m_features.depthBiasClamp
			&& m_features.fillModeNonSolid
			&& m_features.samplerAnisotropy
			&& m_features.sampleRateShading);

		if constexpr (S_enableValidationLayers) // require pipeline statistics queries in Debug mode only (like validation layers)
		{
			hasAllRequiredFeatures &= (bool)m_features.pipelineStatisticsQuery;
		}

		return hasAllRequiredFeatures;
	}


	void MyVkDevice::FetchGraphicsProperties()
	{
		m_properties = m_physicalDevice.getProperties();
	}


	uint32_t MyVkDevice::ComputeGraphicsScore()
	{
		uint32_t score = 0;

		if (m_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
		{
			score += 10000; // huge boost if device is a GPU
		}

		// Get a bonus if this device supports drawing and presentation in the same queue for improved performance.
		if (HasGraphicsAndPresentOnSameQueue())
		{
			score += 1000;
		}

		// Maximum possible size of textures affects graphics quality
		score += m_properties.limits.maxImageDimension2D;
		// Some other factors....
		score += (uint32_t)m_properties.limits.maxSamplerAnisotropy;
		score += m_properties.limits.maxColorAttachments;
		score += m_properties.limits.maxPushConstantsSize;
		score += m_properties.limits.maxSamplerAllocationCount;
		score += m_properties.limits.maxMemoryAllocationCount;
		score += m_properties.limits.maxBoundDescriptorSets;
		score += m_properties.limits.maxVertexInputBindings;
		score += m_properties.limits.maxViewports;
		score += m_properties.limits.maxColorAttachments;

		return score;
	}


	bool MyVkDevice::HasRequiredGraphicsQueueFamilies() const
	{
		return m_queueIndices.IsComplete();
	}

	bool VulkanDeviceList::Initialize(const vk::Instance& instance)
	{
		PopulateDevices(instance);

		return true;
	}


	void VulkanDeviceList::PopulateDevices(const vk::Instance& instance)
	{
		std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
		MOE_DEBUG_ASSERT(false == physicalDevices.empty());

		m_devices.reserve(physicalDevices.size());
		for (auto& vkDevice : physicalDevices)
		{
			auto& device = m_devices.emplace_back(std::move(vkDevice));
			auto& props = device.Properties();


			MOE_INFO(moe::ChanGraphics, "Found physical device %s (%s | Vulkan v%i.%i.%i | Device #%lu)",
				props.deviceName.data(),
				vk::to_string(props.deviceType),
				VK_VERSION_MAJOR(props.apiVersion), VK_VERSION_MINOR(props.apiVersion), VK_VERSION_PATCH(props.apiVersion),
				props.deviceID
			);
		}
	}


	MyVkDevice* VulkanDeviceList::PickGraphicsDevice(vk::SurfaceKHR presentSurface)
	{
		MyVkDevice* bestCandidate = nullptr;
		uint32_t bestScore = 0;

		for (auto& device : m_devices)
		{
			const uint32_t score = device.GetScoreForPresentSurface(presentSurface);
			if (score > bestScore)
			{
				bestScore = score;
				bestCandidate = &device;
			}
		}

		if (MOE_ASSERT(bestCandidate != nullptr))
		{
			m_graphicsDevice = bestCandidate;

			MOE_INFO(moe::ChanGraphics, "Picked %s as my Graphics Device.",
				m_graphicsDevice->Properties().deviceName.data()
			);
		}

		return m_graphicsDevice;
	}

}

#endif // MOE_VULKAN
