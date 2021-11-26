
#ifdef MOE_VULKAN

#include "VulkanDevice.h"

#include "Graphics/Vulkan/ValidationLayers/VulkanValidationLayers.h"

#include "Graphics/Vulkan/Buffer/VulkanBuffer.h"

#include "Graphics/Vulkan/CommandPool/VulkanCommandPool.h"
#include <set>

#include "Core/Misc/Types.h"



namespace moe
{
	MyVkDevice::MyVkDevice(vk::PhysicalDevice&& physDev) :
		m_physicalDevice(std::move(physDev)),
		m_bufferAllocator(*this),
		m_textureAllocator(*this),
		m_memAllocator(*this),
		MeshFactory(*this),
		TextureFactory(*this),
		ShaderFactory(*this)
	{
	}






	MyVkDevice::~MyVkDevice()
	{
		m_logicalDevice.waitIdle();

		m_onDeviceShutdown.Broadcast(*this);

		m_logicalDevice.destroy();
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


	bool MyVkDevice::HasUnifiedGraphicsAndPresentQueues() const
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
		if (HasUnifiedGraphicsAndPresentQueues())
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

		m_logicalDevice = m_physicalDevice.createDevice(deviceCreateInfo);
		MOE_ASSERT((bool)m_logicalDevice);

		m_graphicsQueue = m_logicalDevice.getQueue(m_queueIndices.GraphicsFamilyIdx.value(), 0);
		m_presentQueue = m_logicalDevice.getQueue(m_queueIndices.PresentFamilyIdx.value(), 0);

		return true;
	}


	vk::ImageView MyVkDevice::CreateImageView(vk::Image image, vk::Format format, vk::ImageAspectFlagBits aspectFlags, uint32_t mipLevels) const
	{
		vk::ImageViewCreateInfo createInfo{};
		createInfo.image = image;
		createInfo.viewType = vk::ImageViewType::e2D;
		createInfo.format = format;
		createInfo.subresourceRange.aspectMask = aspectFlags;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = mipLevels;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		vk::ImageView imageView = m_logicalDevice.createImageView(createInfo);
		MOE_ASSERT(imageView);

		return imageView;
	}


	void MyVkDevice::ImmediateCommandSubmit(std::function<void(vk::CommandBuffer)> pushFunction) const
	{
		// Use a transient command pool with one-time submit command buffers to inform the driver this is going to be short-lived
		vk::CommandPoolCreateInfo poolCreateInfo{ vk::CommandPoolCreateFlagBits::eTransient, GraphicsQueueIdx() };
		vk::CommandBufferAllocateInfo cbCreateInfo{ vk::CommandPool(), vk::CommandBufferLevel::ePrimary, 1 };
		VulkanCommandPool tmpPool{ *this, poolCreateInfo, cbCreateInfo };

		auto tmpCmdBuffer = tmpPool.TryGrabCommandBuffer();
		MOE_ASSERT(tmpCmdBuffer.has_value());

		(*tmpCmdBuffer).begin(vk::CommandBufferBeginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

		pushFunction(tmpCmdBuffer.value());

		(*tmpCmdBuffer).end();

		vk::SubmitInfo submitInfo{};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &tmpCmdBuffer.value();

		// Now create a fence to wait on
		vk::FenceCreateInfo fenceCreateInfo{
			vk::FenceCreateFlagBits()
		};

		vk::UniqueFence submitFence = m_logicalDevice.createFenceUnique(fenceCreateInfo);

		MOE_VK_CHECK(GraphicsQueue().submit(1, &submitInfo, submitFence.get()));

		// Now wait for the operation to complete
		static const bool WAIT_ALL = true;
		static const auto NO_TIMEOUT = UINT64_MAX;
		MOE_VK_CHECK(m_logicalDevice.waitForFences(1, &submitFence.get(), WAIT_ALL, NO_TIMEOUT));

		// The operation is over !
	}


	vk::DeviceSize MyVkDevice::GetMinimumAlignment(vk::DescriptorType type) const
	{
		switch (type)
		{
		case vk::DescriptorType::eUniformBufferDynamic:
			return Properties().limits.minUniformBufferOffsetAlignment;
		case vk::DescriptorType::eStorageBufferDynamic:
			return Properties().limits.minStorageBufferOffsetAlignment;
		default:
			MOE_ASSERT(false); // not implemented !
		}

		return 0;
	}

	vk::DeviceSize MyVkDevice::GetMinimumBufferBlockAlignment(vk::BufferUsageFlagBits bufferUsage) const
	{
		if (bufferUsage & vk::BufferUsageFlagBits::eUniformBuffer)
			return Properties().limits.minUniformBufferOffsetAlignment;

		if (bufferUsage & vk::BufferUsageFlagBits::eStorageBuffer)
			return Properties().limits.minStorageBufferOffsetAlignment;

		// I don't know of other alignment restrictions, but there might be...
		return 0;
	}


	bool MyVkDevice::HasRequiredGraphicsQueueFamilies() const
	{
		return m_queueIndices.IsComplete();
	}
}


#endif // MOE_VULKAN
