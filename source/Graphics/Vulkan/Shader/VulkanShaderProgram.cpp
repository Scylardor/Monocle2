
#ifdef MOE_VULKAN

#include "VulkanShaderProgram.h"

#include "Graphics/Vulkan/Device/VulkanDevice.h"

#include "Core/Misc/moeFile.h"

namespace moe
{
	VulkanShaderProgram::VulkanShaderProgram(VulkanShaderProgram&& other) noexcept
	{
		*this = std::move(other);
	}


	VulkanShaderProgram& VulkanShaderProgram::operator=(VulkanShaderProgram&& rhs) noexcept
	{
		if (this != &rhs)
		{
			m_device = rhs.m_device;
			rhs.m_device = nullptr;

			MOE_MOVE(m_vertexBindings);
			MOE_MOVE(m_vertexAttributes);
			MOE_MOVE(m_descriptorSetLayoutInfos);
			MOE_MOVE(m_descriptorSetLayouts);
			MOE_MOVE(m_shaders);
			MOE_MOVE(m_pushConstants);
			MOE_MOVE(m_compiled);
		}

		return *this;
	}


	VulkanShaderProgram::~VulkanShaderProgram()
	{
		if (m_device)
		{
			for (vk::DescriptorSetLayout layout : m_descriptorSetLayouts)
				(*m_device)->destroyDescriptorSetLayout(layout);

			for (auto& shader : m_shaders)
				(*m_device)->destroyShaderModule(shader.Module);
		}
	}


	bool VulkanShaderProgram::Compile(MyVkDevice& device)
	{
		m_device = &device;

		m_descriptorSetLayouts.reserve(m_descriptorSetLayoutInfos.size());

		for (auto& layoutInfo : m_descriptorSetLayoutInfos)
		{
			layoutInfo.LayoutInfo.bindingCount = (uint32_t) layoutInfo.LayoutBindings.size();
			layoutInfo.LayoutInfo.pBindings = layoutInfo.LayoutBindings.data();

			m_descriptorSetLayouts.push_back(device->createDescriptorSetLayout(layoutInfo.LayoutInfo));
			MOE_ASSERT(m_descriptorSetLayouts.back());
		}

		m_compiled = true;
		return m_compiled;
	}



	std::vector<vk::PipelineShaderStageCreateInfo> VulkanShaderProgram::GenerateShaderStageCreateInfo()
	{
		std::vector<vk::PipelineShaderStageCreateInfo> createInfos;
		createInfos.reserve(m_shaders.size());
		for (const auto& shader : m_shaders)
		{
			createInfos.emplace_back(vk::PipelineShaderStageCreateFlags{}, shader.Stage, shader.Module, shader.EntryPoint.c_str());
		}

		return createInfos;
	}


	void VulkanShaderProgram::AddPushConstant(vk::ShaderStageFlags stage, uint32_t offset, uint32_t size)
	{
		m_pushConstants.emplace_back(stage, offset, size);
	}


	void VulkanShaderProgram::AddShaderFile(const MyVkDevice& device, std::string_view SPIRV_filename, vk::ShaderStageFlagBits stage, bool replaceExisting,
		std::string_view entryPoint)
	{
		// check that a shader for the same stage doesn't already exist for this program.
		auto existingIt = std::find_if(m_shaders.begin(), m_shaders.end(), [stage](const auto& shader)
			{
				return (shader.Stage == stage);
			});

		if (existingIt != m_shaders.end() && !replaceExisting)
		{
			MOE_ERROR(moe::ChanGraphics, "Adding a new shader for a stage that already has one for this stage (replaceExisting = false).");
			MOE_ASSERT(false);
			return;
		}

		// read the SPIRV bytecode and create the shader module.
		auto shaderCode = ReadFile(SPIRV_filename, true);
		MOE_ASSERT(shaderCode.has_value());

		vk::ShaderModuleCreateInfo createInfo{};
		createInfo.codeSize = shaderCode->size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode->data());

		vk::ShaderModule shaderModule = device->createShaderModule(createInfo);
		MOE_ASSERT(shaderModule);

		// add the new shader or replace the existing one.
		if (replaceExisting)
		{
			*existingIt = VulkanShader{ shaderModule, stage, entryPoint };
		}
		else
		{
			m_shaders.emplace_back(shaderModule, stage, entryPoint);
		}
	}

	VulkanShaderProgram& VulkanShaderProgram::AddVertexBinding(vk::VertexInputRate inputRate)
	{
		// stride is initialized to 0 and will be updated as we add attributes to this binding
		 m_vertexBindings.emplace_back((uint32_t)m_vertexBindings.size(), 0, inputRate);

		return *this;
	}


	// Assumes we want to add an attribute to the last added binding.
	VulkanShaderProgram& VulkanShaderProgram::AddVertexAttribute(uint32_t location, uint32_t offset, uint32_t size, vk::Format attributeFormat)
	{
		MOE_ASSERT(false == m_vertexBindings.empty()); // cannot add an attribute if there is no binding !

		return AddVertexBindingAttribute(location, m_vertexBindings.back().binding, offset, size, attributeFormat);
	}


	// TODO : The offset could be calculated based on the attribute format... See OpenGLVertexElementFormat::TranslateFormat.
	VulkanShaderProgram& VulkanShaderProgram::AddVertexBindingAttribute(uint32_t location, uint32_t numBinding, uint32_t offset, uint32_t size, vk::Format attributeFormat)
	{
		MOE_ASSERT(m_vertexBindings.size() > numBinding); // cannot add an attribute for a non-existent binding !

		// Update the stride for this binding.
		m_vertexBindings[numBinding].stride += size;

		m_vertexAttributes.emplace_back(location, numBinding, attributeFormat, offset);

		return *this;
	}


	VulkanShaderProgram& VulkanShaderProgram::AddNewDescriptorSetLayout(uint32_t setIndex)
	{
		// http://kylehalladay.com/blog/tutorial/2017/11/27/Vulkan-Material-System.html
		// A VkDescriptorSetLayout struct doesn’t have any spot for specifying which set that layout is for.
		// This means the api assumes that the array of VkDescriptorSetLayouts that you use is a continuous collection of sets
		// - that is if your array is 3 elements long, it is for sets 0, 1, and 2.
		//	In practice, you’ll likely have gaps in the sets that your shaders use, especially if you assign each set number a specific use case.
		// In this case, you need to make a VkDescriptorSet for each set you aren’t using as well.
		// These "empty" elements will have their binding count set to 0, and their pBindings array set to null, but still need to be in your final array of set layouts,
		// or else nothing is going to work right.
		if (setIndex != MONOTONIC_SET_INDEX)
		{
			// make sure there are at least (setIndex + 1) objects in the vector.
			// The newly created ones will have zeroes everywhere and may or may not be used.
			m_descriptorSetLayoutInfos.resize(setIndex + 1);
		}
		else
			m_descriptorSetLayoutInfos.emplace_back();

		return *this;
	}


	VulkanShaderProgram& VulkanShaderProgram::AddNewDescriptorBinding(uint32_t set, uint32_t binding,
		vk::DescriptorType descriptorType, uint32_t descriptorCount, vk::ShaderStageFlagBits stage,
		vk::Sampler* immutableSamplers)
	{
		MOE_ASSERT(m_descriptorSetLayoutInfos.size() > set); // cannot add to a non-existent set !

		m_descriptorSetLayoutInfos[set].LayoutBindings.emplace_back(binding, descriptorType, descriptorCount, stage, immutableSamplers);

		return *this;
	}
}


#endif // MOE_VULKAN
