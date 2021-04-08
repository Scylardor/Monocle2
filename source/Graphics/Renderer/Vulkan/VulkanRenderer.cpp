
#ifdef MOE_VULKAN

#include "Core/Misc/moeFile.h"
#include "VulkanRenderer.h"
#include "Graphics/Vulkan/Surface/VulkanSurfaceProvider.h"

#include "Graphics/Vulkan/MaterialLibrary/VulkanMaterial.h"


const std::vector<moe::VertexData> vertices =
{
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};



const std::vector<uint16_t> indices =
{
	0, 1, 2, 2, 3, 0
};

// This is placeholder, so I disable the "unreferenced formal parameter" warning.
#pragma warning( push )
#pragma warning( disable: 4100 )
namespace moe
{

	VulkanRenderer::VulkanRenderer() :
		m_world(*this)
	{
	}


	bool VulkanRenderer::Initialize(IGraphicsRenderer::GraphicsContextSetup setupFunction)
	{
		return true;
	}

	void VulkanRenderer::Shutdown()
	{
		(*m_graphicsDevice)->waitIdle();
	}

	ShaderProgramHandle VulkanRenderer::CreateShaderProgramFromSource(const ShaderProgramDescriptor& shaProDesc)
	{
		return ShaderProgramHandle::Null();
	}

	ShaderProgramHandle VulkanRenderer::CreateShaderProgramFromBinary(const ShaderProgramDescriptor& shaProDesc)
	{
		return ShaderProgramHandle::Null();
	}

	bool VulkanRenderer::RemoveShaderProgram(ShaderProgramHandle programHandle)
	{
		return true;
	}

	void VulkanRenderer::UseShaderProgram(ShaderProgramHandle shaderProgram)
	{
	}

	VertexLayoutHandle VulkanRenderer::CreateVertexLayout(const VertexLayoutDescriptor& vertexLayoutDesc)
	{
		return VertexLayoutHandle::Null();
	}

	VertexLayoutHandle VulkanRenderer::CreateInstancedVertexLayout(
		const InstancedVertexLayoutDescriptor& vertexLayoutDesc)
	{
		return VertexLayoutHandle::Null();
	}

	DeviceBufferHandle VulkanRenderer::CreateUniformBuffer(const void* data, uint32_t dataSizeBytes)
	{
		return DeviceBufferHandle::Null();
	}

	ResourceLayoutHandle VulkanRenderer::CreateResourceLayout(const ResourceLayoutDescriptor& desc)
	{
		return ResourceLayoutHandle::Null();
	}

	ResourceSetHandle VulkanRenderer::CreateResourceSet(const ResourceSetDescriptor& desc)
	{
		return ResourceSetHandle::Null();
	}

	const IGraphicsDevice& VulkanRenderer::GetGraphicsDevice() const
	{
		return m_device;
	}

	IGraphicsDevice& VulkanRenderer::MutGraphicsDevice()
	{
		return m_device;
	}

	DeviceBufferHandle VulkanRenderer::AllocateObjectMemory(const uint32_t size)
	{
		return DeviceBufferHandle::Null();
	}

	void VulkanRenderer::CopyObjectMemory(DeviceBufferHandle from, uint32_t fromSizeBytes, DeviceBufferHandle to)
	{
	}

	void VulkanRenderer::ReleaseObjectMemory(DeviceBufferHandle freedHandle)
	{
	}

	void VulkanRenderer::UpdateSubBufferRange(DeviceBufferHandle handle, uint32_t offset, void* data, uint32_t dataSize)
	{
	}

	GraphicObjectData VulkanRenderer::ReallocObjectUniformGraphicData(const GraphicObjectData& oldData,
		uint32_t newNeededSize)
	{
		return GraphicObjectData();
	}

	void VulkanRenderer::Clear(const ColorRGBAf& clearColor)
	{
	}

	void VulkanRenderer::ClearDepth()
	{
	}

	void VulkanRenderer::UseMaterial(ShaderProgramHandle progHandle, ResourceSetHandle rscSetHandle)
	{
	}

	void VulkanRenderer::UseMaterial(Material* material)
	{
	}

	void VulkanRenderer::UseMaterialInstance(const MaterialInstance* material)
	{
	}

	void VulkanRenderer::UseMaterialPerObject(Material* material, AGraphicObject& object)
	{
	}

	void VulkanRenderer::BindFramebuffer(FramebufferHandle fbHandle)
	{
	}

	void VulkanRenderer::UnbindFramebuffer(FramebufferHandle fbHandle)
	{
	}


	RenderWorld& VulkanRenderer::CreateRenderWorld()
	{
		return m_world;
	}

	void VulkanRenderer::UseResourceSet(const ResourceSetHandle rscSetHandle)
	{
	}


	bool VulkanRenderer::Initialize(VulkanInstance::CreationParams&& instanceParams, IVulkanSurfaceProvider& surfaceProvider)
	{
		// Steps to initialize the Vulkan RHI:
		//
		// 1: Initialize the Instance.
		bool ok = m_rhi.Initialize(std::move(instanceParams));
		MOE_ASSERT(ok);

		// 2: Retrieve the Vk Surface because we need it to check physical device support for this surface.
		vk::SurfaceKHR presentSurface = surfaceProvider.CreateSurface(m_rhi.GetInstance());

		// 3: Initialize the graphics device (physical and logical device retrieval)
		m_graphicsDevice = m_rhi.InitializeGraphicsDevice(presentSurface);
		MOE_ASSERT(m_graphicsDevice != nullptr);

		// 4: Initialize the swap chain with the previously retrieved surface
		// TODO: probably best to use the RHI for that. Like RHI->SwapchainFactory.Create(SwapChainCreationParams)...
		ok = m_swapchain.Initialize(m_rhi.GetInstance(), *m_graphicsDevice, surfaceProvider, presentSurface);
		MOE_ASSERT(ok);

		// 5: Ready to go...
		ok = m_graphicsDevice->FramebufferFactory.Initialize(*m_graphicsDevice, m_swapchain);
		MOE_ASSERT(ok);

		ok = m_frameGraph.CreateMainRenderPass(*m_graphicsDevice, m_swapchain);
		MOE_ASSERT(ok);

		m_material = CreateMainMaterial();

		CreateCommandPools();

		CreateGeometry();

		return ok;
	}


	VulkanMaterial VulkanRenderer::CreateMainMaterial()
	{

		std::array<vk::DescriptorSetLayoutBinding, 1> bindings = {
			{ { 0, vk::DescriptorType::eUniformBufferDynamic, 1, vk::ShaderStageFlagBits::eVertex, nullptr } }
		};
		vk::DescriptorSetLayoutCreateInfo layoutInfo{
				vk::DescriptorSetLayoutCreateFlags(),
				(uint32_t) bindings.size(),
			bindings.data()
		};


		{
			auto vertShaderCode = ReadFile("source/Graphics/Resources/shaders/Vulkan/vert.spv", true);
			MOE_ASSERT(vertShaderCode.has_value());
			m_vertexShader = CreateShaderModule(vertShaderCode.value());
		}

		vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
		vertShaderStageInfo.module = m_vertexShader.get();
		vertShaderStageInfo.pName = "main";

		{
			auto fragShaderCode = ReadFile("source/Graphics/Resources/shaders/Vulkan/frag.spv", true);
			MOE_ASSERT(fragShaderCode.has_value());
			m_fragmentShader = CreateShaderModule(fragShaderCode.value());
		}

		vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
		fragShaderStageInfo.module = m_fragmentShader.get();
		fragShaderStageInfo.pName = "main";

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		const std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = { vertShaderStageInfo, fragShaderStageInfo };

		std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions = VertexData::GetAttributeDescriptions();
		auto vertexBindingsDescription = VertexData::GetBindingDescription();

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &vertexBindingsDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		// we want to use a viewport that covers the entire framebuffer :
		vk::Viewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)m_swapchain.GetSwapchainImageExtent().width;
		viewport.height = (float)m_swapchain.GetSwapchainImageExtent().height;
		viewport.minDepth = 0.0f; // In VK, normalized depth is [0;1] like in D3D, but this can be changed here.
		viewport.maxDepth = 1.0f;

		// we simply want to draw to the entire framebuffer, so we'll specify a scissor rectangle that covers it entirely:
		vk::Rect2D scissor{
			{ 0, 0 }, m_swapchain.GetSwapchainImageExtent()
		};

		vk::PipelineViewportStateCreateInfo viewportState{};
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		// A limited amount of the state that we've specified in the previous structs can actually be changed without recreating the pipeline.
		// Examples are the size of the viewport, line width and blend constants.
		std::array<vk::DynamicState, 3> dynamicStates = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor,
			vk::DynamicState::eLineWidth
		};
		// disable dynamic state for now
		vk::PipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.dynamicStateCount = 0; //  (uint32_t)dynamicStates.size();
		dynamicState.pDynamicStates = nullptr; // dynamicStates.data();


		// If depthClampEnable = VK_TRUE, fragments that are beyond the near and far planes are clamped
		// to them as opposed to discarding them. This is useful in some special cases like shadow maps.
		vk::PipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.depthClampEnable = VK_FALSE;

		// if rasterizerDiscardEnable = true, geometry never passes through the rasterizer stage. This basically disables any output to the framebuffer.
		rasterizer.rasterizerDiscardEnable = VK_FALSE;

		// display full poygons (use VK_POLYGON_MODE_LINE for wireframe, VK_POLYGON_MODE_POINT for point cloud)
		rasterizer.polygonMode = vk::PolygonMode::eFill;

		// thickness of lines in terms of fragments.
		rasterizer.lineWidth = 1.0f;

		// Activate back-face culling and use clockwise order to determine what is the front face.
		rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		rasterizer.frontFace = vk::FrontFace::eClockwise;

		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		// Let's keep MSAA disabled for now
		vk::PipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		// Add depth and stencil configuration later here...

		// Use alpha blending mode for now
		vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
		colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
		colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
		colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
		colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
		colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

		vk::PipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = vk::LogicOp::eCopy; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		vk::GraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.stageCount = (uint32_t) shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();

		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr; // Not yet
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState; // Optional

		pipelineInfo.renderPass = m_frameGraph.MainRenderPass();
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = vk::Pipeline(); // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		VulkanMaterial matos;

		matos.Initialize(*m_graphicsDevice, m_swapchain, { layoutInfo },
			{ { 0, sizeof(Mat4)} }, pipelineLayoutInfo, pipelineInfo);

		return matos;
	}


	vk::UniqueShaderModule VulkanRenderer::CreateShaderModule(std::string_view bytecode)
	{
		vk::ShaderModuleCreateInfo createInfo{};
		createInfo.codeSize = bytecode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(bytecode.data());

		vk::UniqueShaderModule shaderModule = (*m_graphicsDevice)->createShaderModuleUnique(createInfo);
		MOE_ASSERT(shaderModule);

		return shaderModule;
	}


	void VulkanRenderer::CreateCommandPools()
	{

		m_commandPools.reserve(m_swapchain.GetMaxFramesInFlight());
		for (auto iFrame = 0u; iFrame < m_swapchain.GetMaxFramesInFlight(); iFrame++)
		{
			vk::CommandPoolCreateInfo poolCreateInfo{ vk::CommandPoolCreateFlags(), m_graphicsDevice->GraphicsQueueIdx() };
			vk::CommandBufferAllocateInfo cbCreateInfo{ vk::CommandPool(), vk::CommandBufferLevel::ePrimary, 1 };
			m_commandPools.emplace_back(*m_graphicsDevice, poolCreateInfo, cbCreateInfo);
		}
	}


	void VulkanRenderer::CreateGeometry()
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
		m_geometry = VulkanBuffer::NewVertexBuffer(*m_graphicsDevice, bufferSize, vertices.data());

		bufferSize = sizeof(indices[0]) * indices.size();
		m_geoIndices = VulkanBuffer::NewIndexBuffer(*m_graphicsDevice, bufferSize, indices.data());
	}


	void VulkanRenderer::RenderFrame()
	{
		m_swapchain.PrepareNewFrame();

		auto& thisFrameCommandPool = m_commandPools[m_swapchain.GetFrameIndex()];
		thisFrameCommandPool.Reset(*m_graphicsDevice);

		auto optCb = thisFrameCommandPool.TryGrabCommandBuffer();
		MOE_ASSERT(optCb);
		vk::CommandBuffer renderPassCommandBuffer = optCb.value();

		renderPassCommandBuffer.begin(vk::CommandBufferBeginInfo{  });

		auto& rp = m_frameGraph.MutMainRenderPass();

		rp.Begin(renderPassCommandBuffer);

		m_material.Bind(renderPassCommandBuffer);

		vk::Buffer vertexBuffers[] = { m_geometry };
		vk::DeviceSize offsets[] = { 0 };
		renderPassCommandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
		renderPassCommandBuffer.bindIndexBuffer(m_geoIndices, 0, vk::IndexType::eUint16);
		renderPassCommandBuffer.draw(static_cast<uint32_t>(vertices.size()), 1, 0, 0);
		renderPassCommandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

		rp.End(renderPassCommandBuffer);

		renderPassCommandBuffer.end();

		m_swapchain.SubmitCommandBuffers(&renderPassCommandBuffer, 1);

		m_swapchain.PresentFrame();
	}
}
#pragma warning( pop )

#endif