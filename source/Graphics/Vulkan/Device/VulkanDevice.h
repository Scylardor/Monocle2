#pragma once
#include "Graphics/Vulkan/Allocators/TextureAllocator.h"
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

#include "Graphics/Vulkan/Framebuffer/FramebufferFactory.h"

#include "Graphics/Vulkan/Allocators/BufferAllocator.h"

#include "Graphics/Vulkan/Allocators/MemoryAllocator.h"

#include <optional>

namespace moe
{
	// TODO: Should be renamed VulkanDevice once the placeholder one is gone
	class MyVkDevice
	{
	public:

		struct SwapchainSupportParameters
		{
			vk::SurfaceCapabilitiesKHR			SurfaceCapabilities;
			std::vector<vk::SurfaceFormatKHR>	AvailableSurfaceFormats;
			std::vector<vk::PresentModeKHR>		AvailablePresentModes;
		};

		MyVkDevice(vk::PhysicalDevice&& physDev);

		const auto& Properties() const
		{
			return m_properties;
		}

		const auto& Features() const
		{
			return m_features;
		}

		const auto& GetSwapchainSupportParameters() const
		{
			return m_swapchainSupportParams;
		}

		bool					HasUnifiedGraphicsAndPresentQueues() const;
		bool					HasRequiredGraphicsQueueFamilies() const;
		std::array<uint32_t, 2>	GetQueueFamilyIndicesArray() const
		{
			return { m_queueIndices.GraphicsFamilyIdx.value(), m_queueIndices.PresentFamilyIdx.value() };
		}

		uint32_t	GetScoreForPresentSurface(vk::SurfaceKHR presentSurface);

		bool		CreateLogicalDevice();

		// TODO: temporary, to move somewhere else...
		vk::UniqueImageView		CreateImageView(vk::Image image, vk::Format format, vk::ImageAspectFlagBits aspectFlags, uint32_t mipLevels) const;

		void					ImmediateCommandSubmit(std::function<void(vk::CommandBuffer)> pushFunction) const;

		vk::DeviceSize			GetMinimumAlignment(vk::DescriptorType type) const;

		vk::DeviceSize			GetMinimumBufferBlockAlignment(vk::BufferUsageFlagBits bufferUsage) const;


		vk::Queue	GraphicsQueue() const
		{
			return m_graphicsQueue;
		}

		uint32_t	GraphicsQueueIdx() const
		{
			MOE_ASSERT(m_queueIndices.GraphicsFamilyIdx.has_value());
			return m_queueIndices.GraphicsFamilyIdx.value();
		}

		vk::Queue	PresentQueue() const
		{
			return m_presentQueue;
		}

		VulkanBufferAllocator&	BufferAllocator()
		{
			return m_bufferAllocator;
		}

		VulkanTextureAllocator& TextureAllocator()
		{
			return m_textureAllocator;
		}

		const VulkanTextureAllocator& GetTextureAllocator() const
		{
			return m_textureAllocator;
		}


		VulkanMemoryAllocator&	MemoryAllocator()
		{
			return m_memAllocator;
		}

		vk::PhysicalDevice PhysicalDevice() const
		{
			return m_physicalDevice;
		}



		const vk::Device* operator->() const
		{
			MOE_ASSERT(m_logicalDevice.get());
			return &m_logicalDevice.get();
		}

		vk::Device operator*() const
		{
			MOE_ASSERT(m_logicalDevice.get());
			return m_logicalDevice.get();
		}

		explicit operator vk::Device() const
		{
			MOE_ASSERT(m_logicalDevice.get());
			return m_logicalDevice.get();
		}


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


		bool		FetchGraphicsFeatures();
		void		FetchGraphicsProperties();
		bool		FetchQueueFamilies(vk::SurfaceKHR presentSurface);
		bool		FetchExtensionProperties();
		bool		FetchSwapchainSupportParameters(vk::SurfaceKHR presentSurface);

		uint32_t	ComputeGraphicsScore();


		static vk::PhysicalDeviceFeatures	GetRequiredFeatures();
		static bool							HasRequiredFeatures(const vk::PhysicalDeviceFeatures& features);

		// No Unique handle for Physical Device because it doesn't need to be destroyed
		vk::PhysicalDevice	m_physicalDevice;
		vk::UniqueDevice	m_logicalDevice;

		vk::PhysicalDeviceProperties			m_properties;
		std::vector <vk::ExtensionProperties>	m_extensionProperties;
		vk::PhysicalDeviceFeatures				m_features;
		std::vector<vk::QueueFamilyProperties>	m_queueFamilyProps;

		SwapchainSupportParameters				m_swapchainSupportParams{};



		FamilyIndices	m_queueIndices;
		vk::Queue		m_graphicsQueue;
		vk::Queue		m_presentQueue;

		VulkanBufferAllocator	m_bufferAllocator;
		VulkanTextureAllocator	m_textureAllocator;
		VulkanMemoryAllocator	m_memAllocator;

		// So far, the only extension we have to manage is swap chain.
		static inline const std::array<const char*, 1>	S_neededExtensions =
		{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};


	public:


		FramebufferFactory	FramebufferFactory;

	};


}


#endif // MOE_VULKAN
