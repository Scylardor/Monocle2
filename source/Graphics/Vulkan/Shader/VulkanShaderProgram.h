#pragma once
#ifdef MOE_VULKAN

#include "Core/Resource/ResourceRef.h"
#include "Graphics/Vulkan/VulkanMacros.h"
#include "VulkanShaderModule.h"

#include <string_view>

namespace moe
{
	class VulkanShaderProgram;
	class MyVkDevice;

	struct DescriptorSetLayoutInfo
	{

		vk::DescriptorSetLayoutCreateInfo			LayoutInfo{};
		std::vector<vk::DescriptorSetLayoutBinding>	LayoutBindings{};
	};


	class VulkanShaderProgram
	{
	public:

		using DescriptorSetLayouts = std::vector<vk::DescriptorSetLayout>;
		using DescriptorLayoutInfos = std::vector<DescriptorSetLayoutInfo>;
		using PushConstantRanges = std::vector<vk::PushConstantRange>;

		VulkanShaderProgram() = default;

		VulkanShaderProgram(VulkanShaderProgram&& other) noexcept;
		VulkanShaderProgram& operator=(VulkanShaderProgram&& other) noexcept;

		~VulkanShaderProgram();

		bool	Compile(MyVkDevice& device);


		[[nodiscard]] std::vector< vk::PipelineShaderStageCreateInfo >	GenerateShaderStageCreateInfo();

		void	AddPushConstant(vk::ShaderStageFlags stage, uint32_t offset, uint32_t size);

		void	AddShader(Ref<ShaderResource> addedShaderModule);

		VulkanShaderProgram&	AddVertexBinding(vk::VertexInputRate inputRate = vk::VertexInputRate::eVertex);
		VulkanShaderProgram&	AddVertexAttribute(uint32_t location, uint32_t offset, uint32_t size, vk::Format attributeFormat);
		VulkanShaderProgram&	AddVertexBindingAttribute(uint32_t location, uint32_t numBinding, uint32_t offset, uint32_t size, vk::Format attributeFormat);

		static const uint32_t MONOTONIC_SET_INDEX = UINT32_MAX;
		VulkanShaderProgram&	AddNewDescriptorSetLayout(uint32_t setIndex = MONOTONIC_SET_INDEX);
		VulkanShaderProgram&	AddNewDescriptorBinding(uint32_t set, uint32_t binding, vk::DescriptorType descriptorType, uint32_t descriptorCount, vk::ShaderStageFlagBits stage, vk::Sampler* immutableSamplers = nullptr);

		[[nodiscard]] std::vector<vk::VertexInputBindingDescription> const& VertexBindingDescriptions() const { return m_vertexBindings; }

		[[nodiscard]] std::vector<vk::VertexInputAttributeDescription> const& VertexAttributeDescriptions() const { return m_vertexAttributes; }

		[[nodiscard]] DescriptorSetLayouts const&	GetDescriptorSetLayouts() const { return m_descriptorSetLayouts; }

		[[nodiscard]] DescriptorLayoutInfos const& GetDescriptorSetLayoutInfos() const { return m_descriptorSetLayoutInfos; }

		[[nodiscard]] PushConstantRanges const& PushConstants() const { return m_pushConstants; }

		[[nodiscard]] bool	IsCompiled() const { return m_compiled; }


	private:

		MyVkDevice*											m_device{nullptr};

		std::vector<vk::VertexInputBindingDescription>		m_vertexBindings{};
		std::vector<vk::VertexInputAttributeDescription>	m_vertexAttributes{};

		DescriptorLayoutInfos								m_descriptorSetLayoutInfos{};
		DescriptorSetLayouts								m_descriptorSetLayouts{};

		std::vector<Ref<ShaderResource>>					m_shaders{};

		PushConstantRanges									m_pushConstants{};

		bool												m_compiled = false;
	};
}


#endif // MOE_VULKAN
