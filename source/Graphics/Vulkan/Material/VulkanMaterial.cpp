#ifdef MOE_VULKAN

#include "VulkanMaterial.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"
#include "Graphics/Vulkan/Pipeline/VulkanPipeline.h"

namespace moe
{

	void PhongReflectivityMaterialModule::UpdateResources(MaterialModulesResource& /*updatedMaterial*/)
	{
	}


	void PhongReflectivityMapsMaterialModule::UpdateResources(MaterialModulesResource& updatedMaterial)
	{
		updatedMaterial.BindTextureResource(m_setNumber, (uint8_t)PhongMap::Diffuse, m_maps.Maps[(uint8_t)PhongMap::Diffuse]);
		//for (uint8_t rscIdx = 0; rscIdx < (uint8_t)PhongMap::_COUNT_; ++rscIdx)
		//{
		//	updatedMaterial.BindTextureResource(m_setNumber, rscIdx, m_maps.Maps[rscIdx]);
		//}
	}


	VulkanDescriptorPool::VulkanDescriptorPool(const MyVkDevice& device, const std::vector <vk::DescriptorPoolSize>& poolSizes, uint32_t maxSets)
	{
		const vk::DescriptorPoolCreateInfo poolCreateInfo{
			vk::DescriptorPoolCreateFlags(), maxSets, (uint32_t) poolSizes.size(), poolSizes.data() };

		m_pool = device->createDescriptorPool(poolCreateInfo);

	}


	VulkanMaterial::~VulkanMaterial()
	{
		if (m_device)
		{
			for (auto& pool : m_pools.List)
			{
				(*m_device)->destroyDescriptorPool(pool.Handle()); // will cleanup the allocated descriptor sets.
			}
		}
	}


	std::unique_ptr<MaterialModulesResource> VulkanMaterial::Clone()
	{
		auto matPtr = std::make_unique<VulkanMaterial>();

		matPtr->Initialize(Device(), m_pipeline);

		return std::move(matPtr);
	}


	void VulkanMaterial::AddNewModule(std::unique_ptr<AMaterialModule> newModule)
	{
		m_modules.emplace_back(std::move(newModule));
	}


	VulkanMaterial& VulkanMaterial::Initialize(MyVkDevice& device, Ref<ShaderPipelineResource> pipeline, uint32_t maxInstances)
	{
		m_device = &device;

		m_pipeline = std::move(pipeline);

		// Initialize our descriptor pools based on our shader program descriptor layouts.
		CreateDescriptorSetPool(device, maxInstances);

		AllocateDescriptorSets(device);

		return *this;
	}


	VulkanMaterial& VulkanMaterial::BindTexture(uint32_t set, uint32_t binding, const VulkanTexture& tex)
	{
		const auto bindingIndex = FindBindingDescriptorSetWriteIndex(set, binding);
		MOE_ASSERT(m_writeSets.size() > bindingIndex);

		vk::WriteDescriptorSet& writeSet = m_writeSets[bindingIndex];
		writeSet.dstSet = m_sets[set];
		writeSet.dstBinding = binding;
		writeSet.dstArrayElement = 0;
		writeSet.descriptorType = vk::DescriptorType::eCombinedImageSampler; // TODO: manage other types of textures ?
		writeSet.descriptorCount = 1;
		writeSet.pImageInfo = &tex.DescriptorImageInfo();

		return *this;
	}


	VulkanMaterial& VulkanMaterial::BindUniformBuffer(uint32_t set, uint32_t binding, const VulkanBuffer& buff)
	{
		auto bindingIndex = FindBindingDescriptorSetWriteIndex(set, binding);
		MOE_ASSERT(m_writeSets.size() > bindingIndex);

		vk::WriteDescriptorSet& writeSet = m_writeSets[bindingIndex];
		writeSet.dstSet = m_sets[set];
		writeSet.dstBinding = binding;
		writeSet.dstArrayElement = 0;
		writeSet.descriptorType = vk::DescriptorType::eUniformBuffer;
		writeSet.descriptorCount = 1;
		writeSet.pBufferInfo = &buff.DescriptorBufferInfo();

		return *this;
	}


	VulkanMaterial& VulkanMaterial::BindStorageBuffer(uint32_t set, uint32_t binding, const VulkanBuffer& buff)
	{
		auto bindingIndex = FindBindingDescriptorSetWriteIndex(set, binding);
		MOE_ASSERT(m_writeSets.size() > bindingIndex);

		vk::WriteDescriptorSet& writeSet = m_writeSets[bindingIndex];
		writeSet.dstSet = m_sets[set];
		writeSet.dstBinding = binding;
		writeSet.dstArrayElement = 0;
		writeSet.descriptorType = vk::DescriptorType::eStorageBuffer;
		writeSet.descriptorCount = 1;
		writeSet.pBufferInfo = &buff.DescriptorBufferInfo();

		return *this;
	}


	VulkanMaterial& VulkanMaterial::PushUniformBufferBindingSize(uint8_t set, uint8_t binding, size_t size)
	{
		m_uniformBindingOffsets[(set << 8) | binding] = size;
		m_requiredUniformDataSize += size;
		return *this;
	}


	void VulkanMaterial::UpdateDescriptorSet(uint32_t setNbr)
	{
		Device()->updateDescriptorSets(1, m_writeSets.data() + setNbr, 0, nullptr);
	}


	void VulkanMaterial::UpdateAllDescriptorSets()
	{
		Device()->updateDescriptorSets((uint32_t) m_writeSets.size(), m_writeSets.data(), 0, nullptr);
	}


	void VulkanMaterial::UpdateResources(uint8_t resourceSet)
	{
		MOE_ASSERT(m_modules.size() > resourceSet);
		auto& updatedModule = m_modules[resourceSet];
		updatedModule->UpdateResources(*this);
		UpdateDescriptorSet(resourceSet);
	}


	MaterialModulesResource& VulkanMaterial::BindTextureResource(uint32_t set, uint32_t binding, const Ref<TextureResource>& tex)
	{
		const auto& vkTex = tex.As<VulkanTexture>();

		const auto bindingIndex = FindBindingDescriptorSetWriteIndex(set, binding);
		MOE_ASSERT(m_writeSets.size() > bindingIndex);

		vk::WriteDescriptorSet& writeSet = m_writeSets[bindingIndex];
		writeSet.dstSet = m_sets[set];
		writeSet.dstBinding = binding;
		writeSet.dstArrayElement = 0;
		writeSet.descriptorType = vk::DescriptorType::eCombinedImageSampler; // TODO: manage other types of textures ?
		writeSet.descriptorCount = 1;
		writeSet.pImageInfo = &vkTex.DescriptorImageInfo();

		return *this;
	}


	void VulkanMaterial::Bind(vk::CommandBuffer recordingBuffer) const
	{
		const auto& vkPipeline = m_pipeline.As<VulkanPipeline>();
		recordingBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, vkPipeline.PipelineHandle());
		recordingBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, vkPipeline.PipelineLayout(),
			0, (uint32_t) m_sets.size(), m_sets.data(), 0, nullptr);
	}


	void VulkanMaterial::CreateDescriptorSetPool(const MyVkDevice& device, uint32_t maxInstances)
	{
		// This code takes advantages of the descriptor type enum just being an enumeration starting at 0.
		// WE can then index by descriptor type very easily. But put a future-proof assert here,
		// just in case the Vulkan implementation changes someday. If this breaks, this code is not valid anymore.
		MOE_ASSERT(VK_DESCRIPTOR_TYPE_SAMPLER == 0 && VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT == 10);
		static const uint32_t numberOfDescriptorTypes = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 1;
		std::array<uint32_t, numberOfDescriptorTypes> descriptorCounts{ };

		auto& vkPipeline = m_pipeline.As<VulkanPipeline>();
		for (const auto& layoutInfo : vkPipeline.GetDescriptorSetLayoutInfos())
		{
			for (const auto& binding : layoutInfo.LayoutBindings)
			{
				descriptorCounts[(int)binding.descriptorType] += binding.descriptorCount;
			}
		}

		// Initialize the pool sizes vector once and for all
		// It will be useful to reuse if we need more descriptor pools
		m_pools.PoolSizes.reserve(numberOfDescriptorTypes);
		for (uint32_t iDescType = 0u; iDescType < numberOfDescriptorTypes; iDescType++)
		{
			if (descriptorCounts[iDescType] != 0)
			{
				m_pools.PoolSizes.emplace_back((vk::DescriptorType) iDescType, descriptorCounts[iDescType] * maxInstances);
			}
		}

		// Create a first descriptor pool to begin with
		m_pools.List.emplace_back(device, m_pools.PoolSizes, maxInstances);
	}


	void VulkanMaterial::AllocateDescriptorSets(const MyVkDevice& device)
	{
		vk::DescriptorPool pool = m_pools.List[0].Handle();

		const auto& vkPipeline = m_pipeline.As<VulkanPipeline>();

		const auto& layouts = vkPipeline.GetDescriptorSetLayouts();

		m_sets.resize(layouts.size());

		vk::DescriptorSetAllocateInfo allocInfo{
			pool, (uint32_t) layouts.size(), layouts.data()
		};

		MOE_VK_CHECK(device->allocateDescriptorSets(&allocInfo, m_sets.data()));

		// Calculate how many descriptor set write we need. It's basically the sum of binding counts across all sets
		auto totalBindingsNbr = 0u;
		for (const auto& layoutInfo : vkPipeline.GetDescriptorSetLayoutInfos())
		{
			totalBindingsNbr += (uint32_t) layoutInfo.LayoutBindings.size();
		}

		m_writeSets.resize(totalBindingsNbr);
	}


	uint32_t VulkanMaterial::FindBindingDescriptorSetWriteIndex(uint32_t set, uint32_t binding) const
	{
		MOE_ASSERT(m_pipeline);
		const auto& vkPipeline = m_pipeline.As<VulkanPipeline>();
		const auto& layoutInfos = vkPipeline.GetDescriptorSetLayoutInfos();
		MOE_ASSERT(layoutInfos.size() > set);

		uint32_t setWriteIndex = 0u;
		uint32_t setIndex = 0u;

		while (setIndex < set)
		{
			setWriteIndex += (uint32_t) layoutInfos[setIndex].LayoutBindings.size();

			setIndex++;
		}

		auto bindingIndex = 0;
		while (layoutInfos[setIndex].LayoutBindings[bindingIndex++].binding != binding)
			setWriteIndex++;

		return setWriteIndex;
	}



	// TODO : This is a simplistic example using only one descriptor set.
	bool VulkanMaterial_old::CreateDescriptorSets(const MyVkDevice& device, uint32_t maxFramesInFlight,
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



	bool VulkanMaterial_old::InitializeOffsets(const MyVkDevice& device, const std::vector<vk::DescriptorSetLayoutCreateInfo>& layoutInfos, const DynamicSetsIndices& dynamicSetsIndices,
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


	std::pair<VulkanMaterial_old::DynamicSetsIndices, VulkanMaterial_old::DescriptorTypeCountTable> VulkanMaterial_old::ComputeDynamicSetsIndicesAndDescriptorCounts(
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


	void VulkanMaterial_old::InitializePool(const MyVkDevice& device, uint32_t maxSets, uint32_t maxFramesInFlight, const DescriptorTypeCountTable& descriptorCounts)
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


	std::vector<vk::DescriptorSet> VulkanMaterial_old::AllocateDescriptorSets(const MyVkDevice& device, uint32_t maxFramesInFlight,
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
