#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"
#include "Graphics/Vulkan/Shader/VulkanShaderProgram.h"

namespace moe
{
	class VulkanSwapchain;

	class VulkanPipeline
	{
	public:

		VulkanPipeline& SetShaderProgram(VulkanShaderProgram&& program)
		{
			m_shaderProgram = std::move(program);
			return *this;
		}

		// In VK, normalized depth is [0;1] like in D3D, but this can be changed here.
		VulkanPipeline&	AddViewport(float x, float y, float width, float height, float minDepth = 0.f, float maxDepth = 1.f);
		VulkanPipeline&	AddScissor(vk::Offset2D offset, vk::Extent2D extent);
		VulkanPipeline&	SetFullscreenViewportScissor(const VulkanSwapchain& swapChain);

		VulkanPipeline& SetDepthFeatures(bool enableDepthTest, bool enableDepthWrite);
		VulkanPipeline& SetDepthParameters(vk::CompareOp cmpOp, bool testDepthBounds = false, float minDepthBounds = 0.f, float maxDepthBounds = 1.f);
		VulkanPipeline& SetDepthBias(bool enabled, float constantFactor = 0.f, float clamp = 0.f, float slopeFactor = 0.f);


		VulkanPipeline& SetRasterizerClamping(bool depthClampEnabled, bool rasterizerDiscard);

		VulkanPipeline& SetTopology(vk::PrimitiveTopology newTopo, bool primitiveRestart = false);

		VulkanPipeline& SetCulling(vk::CullModeFlags cullingMode, vk::FrontFace frontFace);

		VulkanPipeline& SetMultisampling(vk::SampleCountFlagBits numSamples, bool enableSampleShading = false, float minSampleShading = 1.f);
		VulkanPipeline& SetMultisamplingAlphaModes(bool alphaToCoverage, bool alphaToOne);

		VulkanPipeline& SetPolygonMode(vk::PolygonMode polyMode);
		VulkanPipeline& SetPolygonLineWidth(float lineWidth);

		VulkanPipeline& EnableBlending(bool enabled);
		VulkanPipeline& SetBlendingParams(vk::BlendFactor srcColorBlendFactor, vk::BlendFactor srcAlphaBlendFactor,
			vk::BlendFactor dstColorBlendFactor, vk::BlendFactor dstAlphaBlendFactor,
			vk::BlendOp colorBlendOp = vk::BlendOp::eAdd, vk::BlendOp alphaBlendOp = vk::BlendOp::eAdd);

		VulkanPipeline& SetRenderPass(vk::RenderPass, uint32_t subpass);

		void	Build(const MyVkDevice& device);


		vk::Pipeline	PipelineHandle() const		{ return m_pipeline.get(); }
		vk::PipelineLayout	PipelineLayout() const	{ return m_pipelineLayout.get(); }


		[[nodiscard]] std::vector<vk::Viewport> const&	Viewports() const { return m_viewports; }
		[[nodiscard]] std::vector<vk::Rect2D> const&	Scissors() const { return m_scissors; }


	private:

		vk::PipelineVertexInputStateCreateInfo		m_vertexInputstateInfo{};
		vk::PipelineInputAssemblyStateCreateInfo	m_inputAssemblystateInfo{};
		vk::PipelineRasterizationStateCreateInfo	m_rasterizationstateInfo{};
		vk::PipelineMultisampleStateCreateInfo		m_multisamplestateInfo{};
		vk::PipelineColorBlendAttachmentState		m_colorBlendAttachmentstateInfo{};
		vk::PipelineColorBlendStateCreateInfo		m_colorBlendstateInfo{};

		vk::PipelineViewportStateCreateInfo			m_viewportStateInfo{};
		std::vector<vk::Viewport>					m_viewports{};
		std::vector<vk::Rect2D>						m_scissors{};

		vk::PipelineDynamicStateCreateInfo			m_dynamicstateInfo{};
		std::vector<vk::DynamicState>				m_dynamicStates{};

		vk::PipelineDepthStencilStateCreateInfo		m_depthStencilStateInfo{};

		VulkanShaderProgram							m_shaderProgram{};

		vk::PipelineLayoutCreateInfo				m_pipelineLayoutInfo{};

		vk::RenderPass	m_renderPass{};
		uint32_t		m_subpass{ 0 };

		vk::UniquePipelineLayout	m_pipelineLayout{};
		vk::UniquePipeline			m_pipeline{};

		// A limited amount of state can actually be changed without recreating the pipeline.
		// Examples are the size of the viewport, line width and blend constants.
		static const std::array<vk::DynamicState, 3>	DEFAULT_DYNAMIC_STATES;
	};


}


#endif // MOE_VULKAN
