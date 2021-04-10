#ifdef MOE_VULKAN

#include "VulkanPipeline.h"

#include "Graphics/Vulkan/Swapchain/VulkanSwapchain.h"

namespace moe
{
	const std::array<vk::DynamicState, 3> VulkanPipeline::DEFAULT_DYNAMIC_STATES = {
		   vk::DynamicState::eViewport,
		   vk::DynamicState::eScissor,
		   vk::DynamicState::eLineWidth
	};

	VulkanPipeline& VulkanPipeline::AddViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
	{
		m_viewports.emplace_back(x, y, width, height, minDepth, maxDepth);
		return *this;
	}

	VulkanPipeline& VulkanPipeline::AddScissor(vk::Offset2D offset, vk::Extent2D extent)
	{
		m_scissors.emplace_back(offset, extent);
		return *this;
	}


	VulkanPipeline& VulkanPipeline::SetFullscreenViewportScissor(const VulkanSwapchain& swapChain)
	{
		const auto& extent = swapChain.GetSwapchainImageExtent();
		AddViewport(0.f, 0.f, (float)extent.width, (float)extent.height, 0.f, 1.f);
		AddScissor({ 0, 0 }, extent);
		return *this;
	}


	VulkanPipeline& VulkanPipeline::SetDepthFeatures(bool enableDepthTest, bool enableDepthWrite)
	{
		m_depthStencilStateInfo.depthTestEnable = enableDepthTest;
		m_depthStencilStateInfo.depthWriteEnable = enableDepthWrite;
		return *this;
	}


	VulkanPipeline& VulkanPipeline::SetDepthParameters(vk::CompareOp cmpOp, bool testDepthBounds, float minDepthBounds,
		float maxDepthBounds)
	{
		m_depthStencilStateInfo.depthCompareOp = cmpOp;
		m_depthStencilStateInfo.depthBoundsTestEnable = testDepthBounds;
		m_depthStencilStateInfo.minDepthBounds = minDepthBounds;
		m_depthStencilStateInfo.maxDepthBounds = maxDepthBounds;

		return *this;
	}


	VulkanPipeline& VulkanPipeline::SetPolygonMode(vk::PolygonMode polyMode)
	{
		m_rasterizationstateInfo.polygonMode = polyMode;
		return *this;
	}

	VulkanPipeline& VulkanPipeline::SetPolygonLineWidth(float lineWidth)
	{
		m_rasterizationstateInfo.lineWidth = lineWidth;
		return *this;
	}


	VulkanPipeline& VulkanPipeline::EnableBlending(bool enabled)
	{
		m_colorBlendAttachmentstateInfo.blendEnable = enabled;
		return *this;
	}


	VulkanPipeline& VulkanPipeline::SetBlendingParams(vk::BlendFactor srcColorBlendFactor,
		vk::BlendFactor srcAlphaBlendFactor, vk::BlendFactor dstColorBlendFactor, vk::BlendFactor dstAlphaBlendFactor,
		vk::BlendOp colorBlendOp, vk::BlendOp alphaBlendOp)
	{
		m_colorBlendAttachmentstateInfo.srcColorBlendFactor = srcColorBlendFactor;
		m_colorBlendAttachmentstateInfo.dstColorBlendFactor = srcAlphaBlendFactor;
		m_colorBlendAttachmentstateInfo.colorBlendOp = colorBlendOp;
		m_colorBlendAttachmentstateInfo.srcAlphaBlendFactor = dstColorBlendFactor;
		m_colorBlendAttachmentstateInfo.dstAlphaBlendFactor = dstAlphaBlendFactor;
		m_colorBlendAttachmentstateInfo.alphaBlendOp = alphaBlendOp;

		return *this;
	}

	VulkanPipeline& VulkanPipeline::SetRenderPass(vk::RenderPass pass, uint32_t subpass)
	{
		m_renderPass = pass;
		m_subpass = subpass;
		return *this;
	}


	VulkanPipeline& VulkanPipeline::SetDepthBias(bool enabled, float constantFactor, float clamp, float slopeFactor)
	{
		m_rasterizationstateInfo.depthBiasEnable = enabled;
		m_rasterizationstateInfo.depthBiasConstantFactor = constantFactor; // Optional
		m_rasterizationstateInfo.depthBiasClamp = clamp; // Optional
		m_rasterizationstateInfo.depthBiasSlopeFactor = slopeFactor; // Optional

		return *this;
	}


	VulkanPipeline& VulkanPipeline::SetRasterizerClamping(bool depthClampEnabled, bool rasterizerDiscard)
	{
		// If depthClampEnable = VK_TRUE, fragments that are beyond the near and far planes are clamped
		// to them as opposed to discarding them. This is useful in some special cases like shadow maps.
		m_rasterizationstateInfo.depthClampEnable = depthClampEnabled;

		// if rasterizerDiscardEnable = true, geometry never passes through the rasterizer stage. This basically disables any output to the framebuffer.
		m_rasterizationstateInfo.rasterizerDiscardEnable = rasterizerDiscard;

		return *this;
	}


	VulkanPipeline& VulkanPipeline::SetTopology(vk::PrimitiveTopology newTopo, bool primitiveRestart)
	{
		m_inputAssemblystateInfo.topology = newTopo;
		m_inputAssemblystateInfo.primitiveRestartEnable = primitiveRestart;
		return *this;
	}


	VulkanPipeline& VulkanPipeline::SetCulling(vk::CullModeFlags cullingMode, vk::FrontFace frontFace)
	{
		m_rasterizationstateInfo.cullMode = cullingMode;
		m_rasterizationstateInfo.frontFace = frontFace;
		return *this;
	}


	VulkanPipeline& VulkanPipeline::SetMultisampling(vk::SampleCountFlagBits numSamples, bool enableSampleShading, float minSampleShading)
	{
		m_multisamplestateInfo.sampleShadingEnable = enableSampleShading;
		m_multisamplestateInfo.rasterizationSamples = numSamples;
		m_multisamplestateInfo.minSampleShading = minSampleShading; // Optional
		m_multisamplestateInfo.pSampleMask = nullptr; // Optional
		return *this;
	}


	VulkanPipeline& VulkanPipeline::SetMultisamplingAlphaModes(bool alphaToCoverage, bool alphaToOne)
	{
		m_multisamplestateInfo.alphaToCoverageEnable = alphaToCoverage; // Optional
		m_multisamplestateInfo.alphaToOneEnable = alphaToOne; // Optional
		return *this;
	}


	void VulkanPipeline::Build(const MyVkDevice& device)
	{
		// Perform some last minute tweaks of values we don't want to let the user set, and a bunch of just-in-time struct filling.

		MOE_ASSERT(m_shaderProgram.IsCompiled()); // or else some initialization step is missing

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.setLayoutCount = (uint32_t)m_shaderProgram.DescriptorSetLayouts().size();
		pipelineLayoutInfo.pSetLayouts = m_shaderProgram.DescriptorSetLayouts().data();
		pipelineLayoutInfo.pushConstantRangeCount = (uint32_t)m_shaderProgram.PushConstants().size();
		pipelineLayoutInfo.pPushConstantRanges = m_shaderProgram.PushConstants().data();

		m_pipelineLayout = device->createPipelineLayoutUnique(pipelineLayoutInfo);
		MOE_ASSERT(m_pipelineLayout);

		vk::GraphicsPipelineCreateInfo pipelineInfo{};

		pipelineInfo.layout = m_pipelineLayout.get();

		auto shaderStageInfo = m_shaderProgram.GenerateShaderStageCreateInfo();
		pipelineInfo.stageCount = (uint32_t)shaderStageInfo.size();
		pipelineInfo.pStages = shaderStageInfo.data();

		m_vertexInputstateInfo.vertexBindingDescriptionCount = (uint32_t) m_shaderProgram.VertexBindingDescriptions().size();
		m_vertexInputstateInfo.pVertexBindingDescriptions = m_shaderProgram.VertexBindingDescriptions().data();
		m_vertexInputstateInfo.vertexAttributeDescriptionCount = (uint32_t) m_shaderProgram.VertexAttributeDescriptions().size();
		m_vertexInputstateInfo.pVertexAttributeDescriptions = m_shaderProgram.VertexAttributeDescriptions().data();
		pipelineInfo.pVertexInputState = &m_vertexInputstateInfo;

		pipelineInfo.pInputAssemblyState = &m_inputAssemblystateInfo;


		m_viewportStateInfo.viewportCount = (uint32_t)m_viewports.size();
		m_viewportStateInfo.pViewports = m_viewports.data();
		m_viewportStateInfo.scissorCount = (uint32_t)m_scissors.size();
		m_viewportStateInfo.pScissors = m_scissors.data();
		pipelineInfo.pViewportState = &m_viewportStateInfo;

		pipelineInfo.pRasterizationState = &m_rasterizationstateInfo;

		pipelineInfo.pMultisampleState = &m_multisamplestateInfo;

		pipelineInfo.pDepthStencilState = &m_depthStencilStateInfo;

		m_colorBlendAttachmentstateInfo.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		m_colorBlendstateInfo.logicOp = vk::LogicOp::eCopy; // Optional
		m_colorBlendstateInfo.attachmentCount = 1;
		m_colorBlendstateInfo.pAttachments = &m_colorBlendAttachmentstateInfo;
		pipelineInfo.pColorBlendState = &m_colorBlendstateInfo;

		// No dynamic states provided ? Use the default ones
		if (false == m_dynamicStates.empty())
		{
			m_dynamicstateInfo.pDynamicStates = m_dynamicStates.data();
			m_dynamicstateInfo.dynamicStateCount = (uint32_t)m_dynamicStates.size();
		}
		else
		{
			m_dynamicstateInfo.pDynamicStates = DEFAULT_DYNAMIC_STATES.data();
			m_dynamicstateInfo.dynamicStateCount = (uint32_t)DEFAULT_DYNAMIC_STATES.size();
		}
		pipelineInfo.pDynamicState = &m_dynamicstateInfo;

		pipelineInfo.renderPass = m_renderPass;
		pipelineInfo.subpass = m_subpass;

		pipelineInfo.basePipelineHandle = vk::Pipeline(); // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		m_pipeline = device->createGraphicsPipelineUnique(vk::PipelineCache(), pipelineInfo);
		MOE_ASSERT(m_pipelineLayout);
	}
}


#endif // MOE_VULKAN
