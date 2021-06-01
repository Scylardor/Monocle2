#ifdef MOE_VULKAN

#include "VulkanDescriptorSetAllocator.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"

namespace moe
{
	VulkanDescriptorSetAllocator::~VulkanDescriptorSetAllocator()
	{
		if (m_device)
		{
			if (m_allocatedLayout)
				Device()->destroyDescriptorSetLayout(m_allocatedLayout); // it contains copies of the same layout

			for (vk::DescriptorPool pool : m_pools)
				Device()->destroyDescriptorPool(pool); // will free the descriptor sets
		}
	}


	void VulkanDescriptorSetAllocator::InitializePool(MyVkDevice& device, const vk::DescriptorSetLayoutCreateInfo& layoutCreateInfo, uint32_t maxSetsPerPool)
	{
		m_device = &device;
		m_maxSetsPerPool = maxSetsPerPool;

		InitializeDescriptorSetLayout(layoutCreateInfo);
	}


	void VulkanDescriptorSetAllocator::InitializeDescriptorSetLayout(const vk::DescriptorSetLayoutCreateInfo& layoutCreateInfo)
	{
		m_layoutCreateInfo = layoutCreateInfo;

		m_allocatedLayout = Device()->createDescriptorSetLayout(m_layoutCreateInfo);
		MOE_ASSERT(m_allocatedLayout);

		// Based on the layout information, compute how many pool sizes we need and how many descriptors there are.
		for (uint32_t iBind = 0u; iBind < m_layoutCreateInfo.bindingCount; iBind++)
		{
			const vk::DescriptorSetLayoutBinding& binding = m_layoutCreateInfo.pBindings[iBind];

			auto sizeIt = std::find_if(m_poolSizes.begin(), m_poolSizes.end(),
				[&binding](const auto& poolSize)
				{
					return (poolSize.type == binding.descriptorType);
				});

			if (sizeIt == m_poolSizes.end())
			{
				m_poolSizes.emplace_back(binding.descriptorType, 1);
			}
			else
			{
				sizeIt->descriptorCount++;
			}
		}
	}


	std::vector<vk::DescriptorSet>	VulkanDescriptorSetAllocator::Allocate(uint32_t nbWantedSets)
	{
		std::vector<vk::DescriptorSet> allocatedSets;

		if (m_availableDescriptorSets.empty() == false)
		{
			// First copy as much available sets as we can
			// (Start from end to use the most recently freed first)
			const auto extractedSets = std::min((uint32_t) m_availableDescriptorSets.size(), nbWantedSets);
			const auto newAvailableSetSize = m_availableDescriptorSets.size() - extractedSets;
			const auto* srcPtr = &m_availableDescriptorSets[newAvailableSetSize];

			allocatedSets.resize(extractedSets);
			std::memcpy(allocatedSets.data(), srcPtr, sizeof(vk::DescriptorSet) * extractedSets);

			// Then erase the taken ones
			m_availableDescriptorSets.resize(m_availableDescriptorSets.size() - extractedSets);
		}

		if (allocatedSets.size() != nbWantedSets)
		{
			// we need MOAR sets but free list is exhausted : we need to create a new pool
			auto oldSize = allocatedSets.size();
			const auto nbConsumedSets = nbWantedSets - oldSize;
			MOE_ASSERT(nbConsumedSets < m_maxSetsPerPool); // check we will allocate enough descriptor sets

			const vk::DescriptorPoolCreateInfo poolCreateInfo{
				vk::DescriptorPoolCreateFlags(),
				m_maxSetsPerPool,
				(uint32_t)m_poolSizes.size(),
				m_poolSizes.data()
			};

			m_pools.emplace_back(Device()->createDescriptorPool(poolCreateInfo));

			// In our case we will create one descriptor set for each swap chain image, all with the same layout.
			// Unfortunately we do need all the copies of the layout because the next function expects an array matching the number of sets.
			const std::vector<vk::DescriptorSetLayout> m_layouts(m_maxSetsPerPool, m_allocatedLayout);

			vk::DescriptorSetAllocateInfo descSetAllocInfo{
				m_pools.back(),
						(uint32_t)m_maxSetsPerPool,
						m_layouts.data()
			};

			const auto setVec = Device()->allocateDescriptorSets(descSetAllocInfo);

			allocatedSets.resize(nbWantedSets);
			// copy the remaining number of sets wanted
			std::memcpy(allocatedSets.data() + oldSize, setVec.data(), sizeof(vk::DescriptorSet) * nbConsumedSets);

			// then put the remaining "unused" sets in the free list.
			if (nbConsumedSets < m_maxSetsPerPool)
			{
				const auto nbRemainingSets = m_maxSetsPerPool - nbConsumedSets;
				oldSize = m_availableDescriptorSets.size();
				m_availableDescriptorSets.resize(oldSize + nbRemainingSets);
				std::memcpy(m_availableDescriptorSets.data() + oldSize, setVec.data() + nbConsumedSets, sizeof(vk::DescriptorSet) * nbRemainingSets);
			}
		}

		return allocatedSets;
	}


	void VulkanDescriptorSetAllocator::Free(vk::DescriptorSet freedSet)
	{
		m_availableDescriptorSets.push_back(freedSet);
	}


	void VulkanDescriptorSetAllocator::AllocateNewDescriptorPool()
	{

	}
}


#endif // MOE_VULKAN
