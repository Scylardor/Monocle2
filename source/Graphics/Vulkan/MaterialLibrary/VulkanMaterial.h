#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

namespace moe
{
	class MyVkDevice;
	class VulkanSwapchain;

	class VulkanMaterial
	{
	public:

		static const uint32_t DEFAULT_MAX_INSTANCES = 64;

		struct BindingSize
		{
			BindingSize(uint32_t binding, size_t size) :
				Binding(binding), Size(size)
			{}

			uint32_t	Binding{};
			size_t		Size{};
		};


		bool	Initialize(const MyVkDevice& device, const VulkanSwapchain& swapChain, const std::vector<vk::DescriptorSetLayoutCreateInfo>& layoutInfos,
							const std::vector<BindingSize>& dynamicBindingSizes,
			vk::PipelineLayoutCreateInfo pipelineLayoutInfo,
			vk::GraphicsPipelineCreateInfo pipelineInfo, uint32_t maxInstances = DEFAULT_MAX_INSTANCES);


		void	Bind(vk::CommandBuffer recordingBuffer) const;

		void	BindDynamicDescriptorSets(vk::CommandBuffer recordingBuffer, uint32_t offsetIdx, uint32_t currentFrameIdx) const;

	protected:

	private:

		typedef	std::vector<int>									DynamicSetsIndices;
		typedef std::unordered_map<vk::DescriptorType, uint32_t>	DescriptorTypeCountTable;

		static std::pair<DynamicSetsIndices, DescriptorTypeCountTable>	ComputeDynamicSetsIndicesAndDescriptorCounts(
			const std::vector<vk::DescriptorSetLayoutCreateInfo>& layoutInfos);

		void	InitializePool(const MyVkDevice& device, uint32_t maxSets, uint32_t maxFramesInFlight, const DescriptorTypeCountTable& descriptorCounts);

		std::vector<vk::DescriptorSet>	AllocateDescriptorSets(const MyVkDevice& device, uint32_t maxFramesInFlight, const std::vector<vk::DescriptorSetLayout>& layouts);


		bool	CreateDescriptorSets(const MyVkDevice& device, uint32_t maxFramesInFlight,
			const std::vector<vk::DescriptorSetLayoutCreateInfo>& layoutInfos,
			const std::vector<vk::DescriptorSetLayout>& layouts,
			const std::vector<BindingSize>& bindingSizes);

		bool	InitializeOffsets(const MyVkDevice& device, const std::vector<vk::DescriptorSetLayoutCreateInfo>& layoutInfos,
			const DynamicSetsIndices& dynamicSetsIndices,
			const std::vector<BindingSize>& dynamicBindingSizes,
			uint32_t maxFramesInFlight, uint32_t maxInstances);

		void	SeparateDescriptorSets(const DynamicSetsIndices& dynamicSetIndices, uint32_t uniqueDescriptorSetsNum, uint32_t maxFramesInFlight);


		static int	DynamicDescriptorIdx(vk::DescriptorType type)
		{
			auto it = std::find(DYNAMIC_DESCRIPTOR_TYPES.begin(), DYNAMIC_DESCRIPTOR_TYPES.end(), type);
			if (it == DYNAMIC_DESCRIPTOR_TYPES.end())
				return -1;
			return (int)(it - DYNAMIC_DESCRIPTOR_TYPES.begin());
		}

		static int	IsDescriptorDynamic(vk::DescriptorType type)
		{
			return DynamicDescriptorIdx(type) != -1;
		}

		std::vector<vk::UniqueDescriptorSetLayout>	m_setLayouts;
		vk::UniquePipeline							m_pipeline;
		vk::UniquePipelineLayout					m_pipelineLayout;



		std::vector<vk::DescriptorSet>		m_staticDescriptorSets;
		std::vector<vk::DescriptorSet>		m_dynamicDescriptorSets;
		vk::Buffer							m_dynamicUniformBuffer;

		std::vector<vk::DeviceSize>			m_dynamicOffsets; // The pack of dynamic offsets used for each dynamic descriptor set provided to each object

		vk::UniqueDescriptorPool			m_pool;
		std::vector<vk::DescriptorSet>		m_descriptorSets;

		uint32_t	m_maxInstances{ 0 };

		inline static const std::array<vk::DescriptorType, 2> DYNAMIC_DESCRIPTOR_TYPES{
			vk::DescriptorType::eUniformBufferDynamic, vk::DescriptorType::eStorageBufferDynamic };
	};
}


#endif // MOE_VULKAN
