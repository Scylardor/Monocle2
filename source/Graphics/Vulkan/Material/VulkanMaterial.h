#pragma once
#include "Math/Vec4.h"

#include "Core/Resource/Material/MaterialResource.h"

namespace moe
{

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


	struct PhongReflectivityParameters
	{
		Vec4	Ambient{ 0.f };
		Vec4	Diffuse{ 0.f }; // AKA "albedo"
		Vec4	Specular{ 0.f };
		Vec4	Emissive{ 0.f };
		float	Opacity = 1.f; // ranges between 0 and 1
		float	SpecularExponent = 0.f; // ranges between 0 and 1000
	};

	enum class PhongMap : uint8_t
	{
		Ambient = 0,
		_FIRST_ = Ambient,
		Diffuse,
		Specular,
		Emissive,
		Shininess,
		_COUNT_
	};

	struct PhongReflectivityMaps
	{
		std::array<Ref<TextureResource>, (size_t)PhongMap::_COUNT_>	Maps{};
	};


	class PhongReflectivityMaterialModule : public AMaterialModule
	{
	public:
		PhongReflectivityMaterialModule(uint8_t set) :
			AMaterialModule(set)
		{}


		[[nodiscard]] PhongReflectivityParameters& operator*()
		{
			return m_params;
		}

		[[nodiscard]] const PhongReflectivityParameters& operator*() const
		{
			return m_params;
		}

		void	UpdateResources(MaterialModulesResource& updatedMaterial) override;


	private:

		PhongReflectivityParameters	m_params;

	};


	class PhongReflectivityMapsMaterialModule : public AMaterialModule
	{
	public:
		PhongReflectivityMapsMaterialModule(uint8_t set) :
			AMaterialModule(set)
		{}


		void	Set(PhongMap type, const Ref<TextureResource>& tex)
		{
			m_maps.Maps[(uint8_t)type] = tex;
		}

		void	UpdateResources(MaterialModulesResource& updatedMaterial) override;



	private:

		PhongReflectivityMaps	m_maps;

	};



}



#ifdef MOE_VULKAN

#include "Core/Resource/Resource.h"
#include "Core/Resource/ResourceRef.h"

#include "Graphics/Vulkan/VulkanMacros.h"
#include "Core/Containers/ObjectPool/ObjectPool.h"
#include "Graphics/Vulkan/Buffer/VulkanBuffer.h"
#include "Graphics/Vulkan/Pipeline/VulkanPipeline.h"

namespace moe
{
	class VulkanBuffer;
	class VulkanTexture;
	class VulkanPipeline;
	class MyVkDevice;
	class VulkanSwapchain;



	class VulkanDescriptorPool
	{
	public:

		VulkanDescriptorPool() = default;

		VulkanDescriptorPool(const MyVkDevice& device, const std::vector <vk::DescriptorPoolSize>& poolSizes, uint32_t maxSets);

		vk::DescriptorPool	Handle() const
		{
			return m_pool;
		}

	private:

		vk::DescriptorPool	m_pool{};
	};


	struct VulkanDescriptorPoolList
	{
	public:

		VulkanDescriptorPoolList() = default;

		std::vector<VulkanDescriptorPool>		List{};
		std::vector <vk::DescriptorPoolSize>	PoolSizes{};

	private:

	};





	class VulkanMaterial : public MaterialModulesResource
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


		~VulkanMaterial();


		/* MaterialModulesResource interface */
		std::unique_ptr<MaterialModulesResource>	Clone() override;

		void								AddNewModule(std::unique_ptr<AMaterialModule> newModule) override;

		void								UpdateResources(uint8_t resourceSet) override;

		MaterialModulesResource&					BindTextureResource(uint32_t set, uint32_t binding, const Ref<TextureResource>& tex) override;
		/* MaterialModulesResource interface end */


		VulkanMaterial& Initialize(MyVkDevice& device, Ref<ShaderPipelineResource> pipeline, uint32_t maxInstances = DEFAULT_MAX_INSTANCES);

		VulkanMaterial& BindTexture(uint32_t set, uint32_t binding, const VulkanTexture& tex);

		VulkanMaterial& BindUniformBuffer(uint32_t set, uint32_t binding, const VulkanBuffer& buff);

		VulkanMaterial& BindStorageBuffer(uint32_t set, uint32_t binding, const VulkanBuffer& buff);

		VulkanMaterial& PushUniformBufferBindingSize(uint8_t set, uint8_t binding, size_t size);

		template <typename T>
		T&				BindAs(uint8_t set, uint8_t binding);


		void	UpdateDescriptorSet(uint32_t setNbr);
		void	UpdateAllDescriptorSets();

		void	Bind(vk::CommandBuffer recordingBuffer) const;


		[[nodiscard]] const VulkanPipeline& GetPipeline() const
		{
			return m_pipeline.As<VulkanPipeline>();
		}

	protected:

	private:

		MOE_VK_DEVICE_GETTER()

		void		CreateDescriptorSetPool(const MyVkDevice& device, uint32_t maxInstances);

		void		AllocateDescriptorSets(const MyVkDevice& device);

		[[nodiscard]] uint32_t	FindBindingDescriptorSetWriteIndex(uint32_t set, uint32_t binding) const;


		MyVkDevice*								m_device{ nullptr };
		Ref<ShaderPipelineResource>				m_pipeline{};
		VulkanDescriptorPoolList				m_pools{};
		std::vector<vk::DescriptorSet>			m_sets;
		std::vector<vk::WriteDescriptorSet>		m_writeSets{};

		std::unordered_map<uint16_t, size_t>	m_uniformBindingOffsets{};
		size_t									m_requiredUniformDataSize{ 0 };

		std::vector<std::unique_ptr<AMaterialModule>>	m_modules{ };

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
