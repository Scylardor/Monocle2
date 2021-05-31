#ifdef MOE_VULKAN

#include "VulkanDescriptorSetAllocator.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"

namespace moe
{
	VulkanDescriptorSetAllocator::~VulkanDescriptorSetAllocator()
	{
		if (m_device)
		{
			if (m_layouts.empty() == false)
				Device()->destroyDescriptorSetLayout(m_layouts[0]); // it contains copies of the same layout

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

		vk::DescriptorSetLayout allocatedLayout = Device()->createDescriptorSetLayout(m_layoutCreateInfo);
		MOE_ASSERT(allocatedLayout);

		m_layouts.resize(m_maxSetsPerPool * m_frameCount, allocatedLayout);

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
			// Capture as much available descriptor sets as we can. Start from end to use the most recently freed first.
			// Since we know vk::DescriptorSets are pointers, we can just make a bulk memcpy + resize.
			auto extractedSets = std::min((uint32_t) m_availableDescriptorSets.size(), nbWantedSets);
			const auto* srcPtr = &m_availableDescriptorSets[m_availableDescriptorSets.size() - extractedSets];

			allocatedSets.resize(extractedSets);
			std::memcpy(allocatedSets.data(), srcPtr, sizeof(vk::DescriptorSet) * extractedSets);
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
