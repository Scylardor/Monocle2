#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"
#include "Core/Containers/ObjectPool/ObjectPool.h"
#include "Math/Vec4.h"
#include "Graphics/Vulkan/Buffer/VulkanBuffer.h"

namespace moe
{
	class VulkanBuffer;
	class VulkanTexture;
	class VulkanPipeline;
	class MyVkDevice;
	class VulkanSwapchain;


	enum class ResourceSet : uint8_t
	{
		PER_FRAME,
		PER_VIEW,
		PER_MATERIAL,
		PER_OBJECT
	};


	enum class MaterialMaps : uint8_t
	{
		AMBIENT,
		DIFFUSE,
		ALBEDO = DIFFUSE,
		SPECULAR,
		NORMAL,
		AMBIENT_OCCLUSION,
		LIGHTMAP = AMBIENT_OCCLUSION,
		SHININESS,
		ROUGHNESS = SHININESS,
		METALLIC,
		EMISSIVE
	};


	enum MaterialSetBindings : uint8_t
	{
		REFLECTANCE_PARAMS,
		_MATERIAL_MAPS_, // Keep at the end
	};


	struct ReflectanceParameters
	{
		Vec4	Ambient{ 0.f };
		Vec4	Diffuse{ 0.f }; // AKA "albedo"
		Vec4	Specular{ 0.f };
	};



	class VulkanDescriptorPool
	{
	public:

		VulkanDescriptorPool() = default;

		VulkanDescriptorPool(const MyVkDevice& device, const std::vector <vk::DescriptorPoolSize>& poolSizes, uint32_t maxSets);

		vk::DescriptorPool	Handle() const
		{
			return m_pool.get();
		}

	private:

		vk::UniqueDescriptorPool	m_pool{};
	};

	class VulkanDescriptorPoolList
	{
	public:

		VulkanDescriptorPoolList() = default;

		std::vector<VulkanDescriptorPool>		List{};
		std::vector <vk::DescriptorPoolSize>	PoolSizes{};

	private:

	};

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

		void	BindPipeline(VulkanPipeline& pipeline);


		VulkanMaterial& Initialize(const MyVkDevice& device, VulkanPipeline& pipeline, uint32_t maxInstances = DEFAULT_MAX_INSTANCES);

		VulkanMaterial& BindTexture(uint32_t set, uint32_t binding, const VulkanTexture& tex);

		VulkanMaterial& BindUniformBuffer(uint32_t set, uint32_t binding, const VulkanBuffer& buff);

		VulkanMaterial& BindStorageBuffer(uint32_t set, uint32_t binding, const VulkanBuffer& buff);

		VulkanMaterial& PushUniformBufferBindingSize(uint8_t set, uint8_t binding, size_t size);

		template <typename T>
		T&				BindAs(uint8_t set, uint8_t binding);


		void	UpdateDescriptorSets(const MyVkDevice& device) ;

		void	Bind(vk::CommandBuffer recordingBuffer) const;

	protected:

	private:

		void		CreateDescriptorSetPool(const MyVkDevice& device, uint32_t maxInstances);

		void		AllocateDescriptorSets(const MyVkDevice& device);

		uint32_t	FindBindingDescriptorSetWriteIndex(uint32_t set, uint32_t binding) const;


		VulkanPipeline*							m_pipeline{ nullptr };
		VulkanDescriptorPoolList				m_pools{};
		std::vector<vk::DescriptorSet>			m_sets;
		std::vector<vk::WriteDescriptorSet>		m_writeSets{};

		std::unordered_map<uint16_t, size_t>	m_uniformBindingOffsets{};
		size_t									m_requiredUniformDataSize{ 0 };

	};




	// Holds :
	// - a pipeline (shaders, etc.)
	// - descriptor set layouts
	class VulkanBaseMaterial
	{
	public:




	protected:

		std::vector<vk::UniqueDescriptorSetLayout>	m_setLayouts;
		vk::UniquePipeline							m_pipeline;
		vk::UniquePipelineLayout					m_pipelineLayout;


	};



	class VulkanMaterialInstance
	{
	public:


		template <typename TParam>
		void	SetMaterialParam(uint32_t binding, const TParam& parameter)
		{

		}


	protected:

		VulkanMaterial* m_baseMaterial = nullptr;

		VulkanBuffer	m_uniformData;

	};





	class VulkanMaterial_old : public VulkanBaseMaterial
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

		void	BindPipeline(VulkanPipeline& pipeline);


		bool	Initialize(const MyVkDevice& device, VulkanPipeline& pipeline, uint32_t maxInstances = DEFAULT_MAX_INSTANCES);


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

		void	CreateDescriptorSetPool(const MyVkDevice& device, uint32_t maxInstances);

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

		VulkanPipeline* m_pipeline{ nullptr };
		VulkanDescriptorPoolList	m_pools{};


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
