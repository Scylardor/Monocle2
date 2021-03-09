
#ifdef MOE_VULKAN

#include "VulkanDevice.h"

#include "Graphics/Vulkan/ValidationLayers/VulkanValidationLayers.h"

#include <set>

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


		// TODO: this could be done a bit better I think ?
		for (auto& ext : S_neededExtensions)
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

		if constexpr (VulkanValidationLayers::AreEnabled()) // require pipeline statistics queries in Debug mode only (like validation layers)
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

	// TODO: may have to be refactored for a nicer interface (that's not very flexible)
	vk::PhysicalDeviceFeatures MyVkDevice::GetRequiredFeatures()
	{
		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.geometryShader = true;
		deviceFeatures.tessellationShader = true;
		deviceFeatures.multiDrawIndirect = true;
		deviceFeatures.multiViewport = true;
		deviceFeatures.depthClamp = true;
		deviceFeatures.depthBiasClamp = true;
		deviceFeatures.fillModeNonSolid = true;
		deviceFeatures.samplerAnisotropy = true;
		deviceFeatures.sampleRateShading = VK_TRUE;

		return deviceFeatures;
	}


	bool MyVkDevice::HasRequiredFeatures(const vk::PhysicalDeviceFeatures& features)
	{
		const bool hasAllRequiredFeatures = (
			features.geometryShader
			&& features.tessellationShader
			&& features.multiDrawIndirect
			&& features.multiViewport
			&& features.depthClamp
			&& features.depthBiasClamp
			&& features.fillModeNonSolid
			&& features.samplerAnisotropy
			&& features.sampleRateShading);

		return hasAllRequiredFeatures;
	}


	bool MyVkDevice::CreateLogicalDevice()
	{
		// what we're interested in is creating graphics devices
		MOE_ASSERT(HasRequiredGraphicsQueueFamilies());

		// Vulkan lets you assign priorities between 0.0 and 1.0 to influence the scheduling of command buffer execution on specific queues.
		// We don't really need this feature but this is required even if there is only a single queue.
		static const float queuePriority = 1.0f;

		// Create one device queue per family index we need.
		// This index may be the same for multiple families so we need to "filter" them to only keep the unique ones.
		// Set also has the nice property to keep them ordered.
		std::set<uint32_t> uniqueQueueFamilies = { m_queueIndices.GraphicsFamilyIdx.value(), m_queueIndices.PresentFamilyIdx.value() };

		// We don't really need more than one queue because you can record all of the command buffers in parallel with multithreading.
		// They will all get submitted in one call.
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos(uniqueQueueFamilies.size());
		int iInfo = 0;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			auto& queueCreateInfo = queueCreateInfos[iInfo];
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			iInfo++;
		}

		vk::DeviceCreateInfo deviceCreateInfo{};

		deviceCreateInfo.queueCreateInfoCount = (uint32_t) queueCreateInfos.size();
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

		vk::PhysicalDeviceFeatures deviceFeatures = GetRequiredFeatures();
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

		deviceCreateInfo.enabledExtensionCount = (uint32_t) S_neededExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = S_neededExtensions.data();

		if constexpr (VulkanValidationLayers::AreEnabled())
		{
			deviceCreateInfo.enabledLayerCount = (uint32_t)VulkanValidationLayers::Names().size();
			deviceCreateInfo.ppEnabledLayerNames = VulkanValidationLayers::Names().data();
		}

		m_logicalDevice = m_physicalDevice.createDeviceUnique(deviceCreateInfo);
		MOE_ASSERT((bool)m_logicalDevice.get());

		m_graphicsQueue = m_logicalDevice->getQueue(m_queueIndices.GraphicsFamilyIdx.value(), 0);
		m_presentQueue = m_logicalDevice->getQueue(m_queueIndices.PresentFamilyIdx.value(), 0);

		return true;
	}


	bool MyVkDevice::HasRequiredGraphicsQueueFamilies() const
	{
		return m_queueIndices.IsComplete();
	}
}


#endif // MOE_VULKAN
