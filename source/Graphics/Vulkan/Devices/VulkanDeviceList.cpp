#ifdef MOE_VULKAN

#include "VulkanDeviceList.h"
#include "Graphics/Vulkan/VulkanMacros.h"

namespace moe
{
	VkDevice::VkDevice(vk::PhysicalDevice&& physDev) :
		m_physicalDevice(std::move(physDev))
	{
		m_properties = m_physicalDevice.getProperties();

		m_features = m_physicalDevice.getFeatures();

		m_queueFamilyProps = m_physicalDevice.getQueueFamilyProperties();
	}


	uint32_t VkDevice::GetGraphicsScore() const
	{
		if (false == (HasRequiredGraphicsFeatures() && HasRequiredGraphicsQueueFamily()))
			return 0;

		uint32_t score = RateGraphicsProperties();
		return score;
	}


	bool VkDevice::HasRequiredGraphicsFeatures() const
	{
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


	uint32_t VkDevice::RateGraphicsProperties() const
	{
		uint32_t score = 0;

		if (m_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
		{
			score += 10000; // huge boost if device is a GPU
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


	bool VkDevice::HasRequiredGraphicsQueueFamily() const
	{
		for (const auto& queueFamilyProp : m_queueFamilyProps)
		{
			if (queueFamilyProp.queueFlags & vk::QueueFlagBits::eGraphics)
			{
				return true;
			}
		}

		return false;
	}

	bool VulkanDeviceList::Initialize(const vk::Instance& instance)
	{
		PopulateDevices(instance);

		m_graphicsDevice = PickGraphicsDevice();

		if (MOE_ASSERT(m_graphicsDevice != nullptr))
		{
			MOE_INFO(moe::ChanGraphics, "Picked %s as my Graphics Device.",
				m_graphicsDevice->Properties().deviceName.data()
			);
		}

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


	VkDevice* VulkanDeviceList::PickGraphicsDevice()
	{
		VkDevice* bestCandidate = nullptr;
		uint32_t bestScore = 0;

		for (auto& device : m_devices)
		{
			const uint32_t score = device.GetGraphicsScore();
			if (score > bestScore)
			{
				bestScore = score;
				bestCandidate = &device;
			}
		}

		return bestCandidate;
	}

}

#endif // MOE_VULKAN
