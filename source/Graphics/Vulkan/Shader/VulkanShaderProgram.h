#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

#include <string_view>

namespace moe
{
	class VulkanShaderProgram;
	class MyVkDevice;

	// TODO : I'd like to get rid of the vk::UniqueShaderModule so I can directly store PipelineShaderStageCreateInfo's and get rid of GenerateShaderStageCreateInfo()!
	struct VulkanShader
	{
		VulkanShader(vk::UniqueShaderModule module, vk::ShaderStageFlagBits stage, std::string_view entryPoint) :
			Module(std::move(module)), Stage(stage), EntryPoint(entryPoint)
		{}

		vk::UniqueShaderModule	Module{};
		vk::ShaderStageFlagBits	Stage{};
		std::string				EntryPoint{ "main" };
	};


	struct DescriptorSetLayoutInfos
	{
		vk::DescriptorSetLayoutCreateInfo			LayoutInfo{};
		std::vector<vk::DescriptorSetLayoutBinding>	LayoutBindings{};
		vk::UniqueDescriptorSetLayout				LayoutHandle{}; // TODO: Get rid of the unique handles so that there's no duplication with m_descriptorSetLayouts.
	};


	class VulkanShaderProgram
	{
	public:

		bool	Compile(const MyVkDevice& device);


		[[nodiscard]] std::vector< vk::PipelineShaderStageCreateInfo >	GenerateShaderStageCreateInfo();

		void	AddPushConstant(vk::ShaderStageFlags stage, uint32_t offset, uint32_t size);

		void	AddShaderFile(const MyVkDevice& device, std::string_view SPIRV_filename, vk::ShaderStageFlagBits stage, bool replaceExisting = false, std::string_view entryPoint = "main");

		VulkanShaderProgram&	AddVertexBinding(vk::VertexInputRate inputRate = vk::VertexInputRate::eVertex);
		VulkanShaderProgram&	AddVertexAttribute(uint32_t location, uint32_t offset, uint32_t size, vk::Format attributeFormat);
		VulkanShaderProgram&	AddVertexBindingAttribute(uint32_t location, uint32_t numBinding, uint32_t offset, uint32_t size, vk::Format attributeFormat);

		static const uint32_t MONOTONIC_SET_INDEX = UINT32_MAX;
		VulkanShaderProgram&	AddNewDescriptorSetLayout(uint32_t setIndex = MONOTONIC_SET_INDEX);
		VulkanShaderProgram&	AddNewDescriptorBinding(uint32_t set, uint32_t binding, vk::DescriptorType descriptorType, uint32_t descriptorCount, vk::ShaderStageFlagBits stage, vk::Sampler* immutableSamplers = nullptr);

		[[nodiscard]] std::vector<vk::VertexInputBindingDescription> const& VertexBindingDescriptions() const { return m_vertexBindings; }

		[[nodiscard]] std::vector<vk::VertexInputAttributeDescription> const& VertexAttributeDescriptions() const { return m_vertexAttributes; }

		[[nodiscard]] std::vector<vk::DescriptorSetLayout> const&	DescriptorSetLayouts() const { return m_descriptorSetLayouts; }

		[[nodiscard]] std::vector<vk::PushConstantRange> const& PushConstants() const { return m_pushConstants; }

		bool	IsCompiled() const { return m_compiled; }


	private:

		std::vector<vk::VertexInputBindingDescription>		m_vertexBindings{};
		std::vector<vk::VertexInputAttributeDescription>	m_vertexAttributes{};

		std::vector<DescriptorSetLayoutInfos>				m_descriptorSetLayoutInfos{};
		std::vector<vk::DescriptorSetLayout>				m_descriptorSetLayouts{};

		std::vector<VulkanShader>	m_shaders{};

		std::vector<vk::PushConstantRange>	m_pushConstants{};

		bool	m_compiled = false;
	};
}


#endif // MOE_VULKAN
