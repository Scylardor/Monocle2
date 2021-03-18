#ifdef MOE_VULKAN

#include "VulkanMaterial.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"
#include "Graphics/Vulkan/Swapchain/VulkanSwapchain.h"

namespace moe
{
	bool VulkanMaterial::Initialize(const MyVkDevice& device, const VulkanSwapchain& swapChain, const std::vector<vk::DescriptorSetLayoutCreateInfo>& layoutInfos,
		const std::vector<BindingSize>& bindingSizes,
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo,
		vk::GraphicsPipelineCreateInfo pipelineInfo, uint32_t maxInstances)
	{
		m_setLayouts.reserve(layoutInfos.size());
		std::vector<vk::DescriptorSetLayout> pipelineSetLayouts;
		pipelineSetLayouts.reserve(layoutInfos.size());
		for (const auto & layoutInfo : layoutInfos)
		{
			m_setLayouts.emplace_back(device->createDescriptorSetLayoutUnique(layoutInfo));
			pipelineSetLayouts.push_back(m_setLayouts.back().get());
		}

		pipelineLayoutInfo.setLayoutCount = (uint32_t) pipelineSetLayouts.size();
		pipelineLayoutInfo.pSetLayouts = pipelineSetLayouts.data();

		m_pipelineLayout = device->createPipelineLayoutUnique(pipelineLayoutInfo);
		MOE_ASSERT(m_pipelineLayout);

		pipelineInfo.layout = m_pipelineLayout.get();

		m_pipeline = device->createGraphicsPipelineUnique(vk::PipelineCache(), pipelineInfo);
		MOE_ASSERT(m_pipeline);

		m_maxInstances = maxInstances;

		auto maxFramesInFlight = swapChain.GetMaxFramesInFlight();

		CreateDescriptorSets(device, maxFramesInFlight, layoutInfos, pipelineSetLayouts, bindingSizes);



		return true;
	}

	void VulkanMaterial::Bind(vk::CommandBuffer recordingBuffer) const
	{
		recordingBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline.get());

		if (m_staticDescriptorSets.empty())
			return;

		// No dynamic offsets for static descriptor sets
		recordingBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout.get(),
			0, (uint32_t)m_staticDescriptorSets.size(), m_staticDescriptorSets.data(),
			0, nullptr);
	}




	// TODO : This is a simplistic example using only one descriptor set.
	bool VulkanMaterial::CreateDescriptorSets(const MyVkDevice& device, uint32_t maxFramesInFlight,
		const std::vector<vk::DescriptorSetLayoutCreateInfo>& layoutInfos,
		const std::vector<vk::DescriptorSetLayout>& layouts,
		const std::vector<BindingSize>& bindingSizes)
	{
		// We need to buffer descriptor sets based on how much frames will be in flight
		// We consider that we need (max images in flight) descriptors sets (without dynamic bindings)
		// and (max images in flight * maxInstances) dynamic descriptor sets (with dynamic bindings).
		auto [dynamicSetsIndices, descriptorCounts] = ComputeDynamicSetsIndicesAndDescriptorCounts(layoutInfos);

		uint32_t maxSets = (uint32_t)layoutInfos.size() * maxFramesInFlight;

		InitializePool(device, maxSets, maxFramesInFlight, descriptorCounts);

		// Now that the pool is initialized, we can allocate our descriptor sets, and segregate between static and dynamic ones.
		m_descriptorSets = AllocateDescriptorSets(device, maxFramesInFlight, layouts);

		auto nbStaticSets = (layouts.size() - dynamicSetsIndices.size()) * maxFramesInFlight;
		if (nbStaticSets > 0) // allocate from the pool !
		{
			m_staticDescriptorSets.reserve(nbStaticSets);
			m_staticDescriptorSets.insert(m_staticDescriptorSets.begin(), m_descriptorSets.begin() + dynamicSetsIndices.size(), m_descriptorSets.end());
		}

		auto nbDynamicSets = (dynamicSetsIndices.size() * maxFramesInFlight);
		m_dynamicDescriptorSets.reserve(nbDynamicSets);
		m_dynamicDescriptorSets.insert(m_dynamicDescriptorSets.begin(), m_descriptorSets.begin(), m_descriptorSets.begin() + dynamicSetsIndices.size());

		auto nbAllocatedSets = m_dynamicDescriptorSets.size() + m_staticDescriptorSets.size();
		auto firstAllocatedSet = m_descriptorSets.size() - nbAllocatedSets;
		m_descriptorSets.erase(m_descriptorSets.begin() + firstAllocatedSet, m_descriptorSets.end()); // will probably be empty at the end since we allocated the exact number of required sets.


		InitializeOffsets(device, layoutInfos, dynamicSetsIndices, bindingSizes, maxFramesInFlight, m_maxInstances);
		return true;
	}


	bool VulkanMaterial::InitializeOffsets(const MyVkDevice& device, const std::vector<vk::DescriptorSetLayoutCreateInfo>& layoutInfos, const DynamicSetsIndices& dynamicSetsIndices,
		const std::vector<BindingSize>& dynamicBindingSizes, uint32_t maxFramesInFlight, uint32_t maxInstances)
	{
		// I want to know which dynamic type it is (uniform or storage)
		// TODO: this could have been done during dynamic set indices harvesting !
		std::vector<vk::DescriptorType> dynamicBindingTypes;
		dynamicBindingTypes.reserve(dynamicBindingSizes.size());

		for (auto dynSetIdx : dynamicSetsIndices)
		{
			for (auto iBinding = 0u; iBinding < layoutInfos[dynSetIdx].bindingCount; iBinding++)
			{
				const auto& binding = layoutInfos[dynSetIdx].pBindings[iBinding];
				if (IsDescriptorDynamic(binding.descriptorType))
					dynamicBindingTypes.push_back(binding.descriptorType);
			}
		}

		// Now calculate each offset aligned to the needs of the underlying storage (uniform or storage).
		vk::DeviceSize currentOffset = 0;

		// I'm going to need (num dynamic bindings) * maxFramesinFlight per user.
		m_dynamicOffsets.reserve(dynamicBindingSizes.size() * maxFramesInFlight * maxInstances);

		for (auto iInst = 0u; iInst < maxInstances; iInst++)
		{
			for (auto iFrame = 0u; iFrame < maxFramesInFlight; iFrame++)
			{
				for (auto iBinding = 0u; iBinding < dynamicBindingSizes.size(); ++iBinding)
				{
					m_dynamicOffsets.push_back(currentOffset);

					// now compute how far we should push the offset, taking alignment into account
					auto alignedBindingSize = dynamicBindingSizes[iBinding].Size;
					auto align = device.GetMinimumAlignment(dynamicBindingTypes[iBinding]);
					if (align > 0)
						alignedBindingSize = (alignedBindingSize + align - 1) & ~(align - 1);
					currentOffset += alignedBindingSize;
				}
			}
		}

		return true;
	}


	std::pair<VulkanMaterial::DynamicSetsIndices, VulkanMaterial::DescriptorTypeCountTable> VulkanMaterial::ComputeDynamicSetsIndicesAndDescriptorCounts(
		const std::vector<vk::DescriptorSetLayoutCreateInfo>& layoutInfos)
	{
		// Based on the number of descriptor sets and types of bindings we can know what to ask from the descriptor pool.
		// We also figure out which of our descriptor sets are dynamic or not (will help to segregate them later).
		DescriptorTypeCountTable descriptorCounts;

		// It will help us to know which sets are dynamic and which are not.
		std::vector<int> dynamicSetsIndices;
		dynamicSetsIndices.reserve(layoutInfos.size()); // a bit wasteful but temporary

		int iSet = 0;
		for (const auto& layoutInfo : layoutInfos)
		{
			bool isDynamic = false;

			for (auto iBinding = 0u; iBinding < layoutInfo.bindingCount; iBinding++)
			{
				const auto& bindingInfo = layoutInfo.pBindings[iBinding];
				// Add (descriptor count) to this descriptor type
				auto& bindingCount = descriptorCounts[bindingInfo.descriptorType];
				bindingCount += bindingInfo.descriptorCount;

				isDynamic = IsDescriptorDynamic(bindingInfo.descriptorType);
			}

			if (isDynamic)
				dynamicSetsIndices.push_back(iSet);

			iSet++;
		}

		return { dynamicSetsIndices, descriptorCounts };
	}


	void VulkanMaterial::InitializePool(const MyVkDevice& device, uint32_t maxSets, uint32_t maxFramesInFlight, const DescriptorTypeCountTable& descriptorCounts)
	{
		// Now that we have the total number of bindings we will want, we can generate all the pool sizes we need...
		std::vector<vk::DescriptorPoolSize> poolSizes;
		poolSizes.reserve(descriptorCounts.size());
		for (const auto& [descriptorType, descriptorCount] : descriptorCounts)
		{
			poolSizes.emplace_back(descriptorType, descriptorCount * maxFramesInFlight);
		}

		// DescriptorPoolCreateInfo has an optional flag similar to command pools
		// that determines if individual descriptor sets can be freed or not:
		// VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT.
		// We're not going to touch the descriptor set after creating it, so we don't need this flag.
		vk::DescriptorPoolCreateInfo poolCreateInfo{ vk::DescriptorPoolCreateFlags(),
			maxSets,
			(uint32_t)poolSizes.size(), poolSizes.data()
		};

		m_pool = device->createDescriptorPoolUnique(poolCreateInfo);
	}


	std::vector<vk::DescriptorSet> VulkanMaterial::AllocateDescriptorSets(const MyVkDevice& device, uint32_t maxFramesInFlight,
		const std::vector<vk::DescriptorSetLayout>& layouts)
	{
		// It's silly but the number of provided layouts and the number of descriptor sets to create must match...
		// given we want to duplicate them for multiple frames in flight, we need to repeat the layouts vector.
		std::vector<vk::DescriptorSetLayout> setLayouts;
		setLayouts.reserve(layouts.size() * maxFramesInFlight);
		for (auto iFrame = 0u; iFrame < maxFramesInFlight; iFrame++)
		{
			setLayouts.insert(setLayouts.end(), layouts.begin(), layouts.end());
		}

		// now we can allocate our descriptor sets
		const vk::DescriptorSetAllocateInfo allocInfo{
			m_pool.get(),
			(uint32_t) setLayouts.size(),
			setLayouts.data()
		};

		std::vector<vk::DescriptorSet> descriptorSets = device->allocateDescriptorSets(allocInfo);
		return descriptorSets;
	}
}


#endif // MOE_VULKAN
