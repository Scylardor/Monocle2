
#ifdef MOE_VULKAN

#include "VulkanRenderer.h"

#include "Core/Resource/ResourceManager.h"
#include "Graphics/Vulkan/Surface/VulkanSurfaceProvider.h"

#include "Graphics/Vulkan/Shader/VulkanShaderProgram.h"
#include "Graphics/Vertex/VertexFormats.h"

// This is placeholder, so I disable the "unreferenced formal parameter" warning.
#pragma warning( push )
#pragma warning( disable: 4100 )
namespace moe
{
	vk::VertexInputBindingDescription GetBasicVertexBindingDescription()
	{
		vk::VertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(BasicVertex);
		bindingDescription.inputRate = vk::VertexInputRate::eVertex;

		return bindingDescription;
	}

	std::array<vk::VertexInputAttributeDescription, 3> GetBasicVertexAttributeDescriptions()
	{
		std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
		attributeDescriptions[0].offset = offsetof(BasicVertex, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
		attributeDescriptions[1].offset = offsetof(BasicVertex, Color);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 2;
		attributeDescriptions[1].format = vk::Format::eR32G32Sfloat;
		attributeDescriptions[1].offset = offsetof(BasicVertex, Texture_UV0);

		return attributeDescriptions;
	}

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


		CreateCommandPools();

		return ok;
	}


	void VulkanRenderer::CreateMainMaterial()
	{
		// First create our texture
		VulkanTextureBuilder texBuilder{};
		m_materialTexture = VulkanTexture::Create2DFromFile(*m_graphicsDevice, "Sandbox/assets/textures/texture.jpg", texBuilder);

		VulkanShaderProgram program;

		const auto shaderBytecodeLoaderFn = [this](std::string_view filename, vk::ShaderStageFlagBits shaderStage)
		{
			return [this, filename, shaderStage]()
			{
				return GraphicsDevice().ShaderFactory.LoadShaderBytecode(filename, shaderStage);
			};
		};
		auto vertShader = m_resourceManager->Load<ShaderResource>(HashString("vert.spv"), shaderBytecodeLoaderFn("source/Graphics/Resources/shaders/Vulkan/vert.spv", vk::ShaderStageFlagBits::eVertex));
		auto fragShader = m_resourceManager->Load<ShaderResource>(HashString("frag.spv"), shaderBytecodeLoaderFn("source/Graphics/Resources/shaders/Vulkan/frag.spv", vk::ShaderStageFlagBits::eFragment));


		program.AddShader(std::move(vertShader));
		program.AddShader(std::move(fragShader));
		program.AddPushConstant(vk::ShaderStageFlagBits::eVertex, 0, sizeof(Mat4)); // object mvp
		program.AddVertexBinding(vk::VertexInputRate::eVertex)
			.AddVertexAttribute(0, offsetof(BasicVertex, Position), sizeof(BasicVertex::Position), vk::Format::eR32G32B32Sfloat)
			.AddVertexAttribute(1, offsetof(BasicVertex, Color), sizeof(BasicVertex::Color), vk::Format::eR32G32B32Sfloat)
			.AddVertexAttribute(2, offsetof(BasicVertex, Texture_UV0), sizeof(BasicVertex::Texture_UV0), vk::Format::eR32G32Sfloat)
			.AddNewDescriptorSetLayout()
				//.AddNewDescriptorBinding(0, 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex)
				.AddNewDescriptorBinding(0, 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment)
			.Compile(*m_graphicsDevice);

		auto pipelineRef = m_resourceManager->Load<ShaderPipelineResource>(HashString("DefaultPipeline"), [&]()
			{
				return std::make_unique<VulkanPipeline>();
			});

		pipelineRef.As<VulkanPipeline>().SetShaderProgram(std::move(program))
			.SetDepthFeatures(true, true)
			.SetDepthParameters(vk::CompareOp::eLess)
			.SetTopology(vk::PrimitiveTopology::eTriangleList, VK_FALSE)
			.SetFullscreenViewportScissor(m_swapchain)
			.SetRasterizerClamping(false, false)
			.SetPolygonMode(vk::PolygonMode::eFill)
			.SetPolygonLineWidth(1.f)
			.SetCulling(vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise)
			.SetMultisampling(VulkanTexture::MAX_SAMPLES, true, 0.5f)
			.SetRenderPass(m_frameGraph.MainRenderPass(), 0)
			.Build(*m_graphicsDevice);

		m_material.Initialize(*m_graphicsDevice, std::move(pipelineRef))
			.BindTexture(0, 1, m_materialTexture)
			.UpdateDescriptorSets(*m_graphicsDevice);
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




	void VulkanRenderer::RenderFrame(const RenderScene& renderedScene)
	{
		m_swapchain.PrepareNewFrame();

		auto& thisFrameCommandPool = m_commandPools[m_swapchain.GetFrameIndex()];
		thisFrameCommandPool.Reset(*m_graphicsDevice);

		auto optCb = thisFrameCommandPool.TryGrabCommandBuffer();
		MOE_ASSERT(optCb);
		vk::CommandBuffer renderPassCommandBuffer = optCb.value();

		renderPassCommandBuffer.begin(vk::CommandBufferBeginInfo{  });

		auto& rp = m_frameGraph.MutMainRenderPass();

		rp.Begin(renderPassCommandBuffer, m_swapchain.GetImageInFlightIndex());

		const VulkanPipeline& pipeline = m_material.GetPipeline();

		renderPassCommandBuffer.setViewport(0, (uint32_t)pipeline.Viewports().size(), pipeline.Viewports().data());
		renderPassCommandBuffer.setScissor(0, (uint32_t)pipeline.Scissors().size(), pipeline.Scissors().data());

		uint32_t lastMatID = ~0u;

		for (const auto& drawable : renderedScene)
		{
			auto matID = drawable.GetMaterialID();
			if (lastMatID != matID && matID == 0)
			{
				m_material.Bind(renderPassCommandBuffer);
				lastMatID = matID;
			}

			drawable.BindTransform(pipeline, renderPassCommandBuffer);

			const auto& drawableMesh = m_resourceManager->GetResourceAs< VulkanMesh >(drawable.GetMeshID());

			drawableMesh.Draw(renderPassCommandBuffer);
		}

		rp.End(renderPassCommandBuffer);

		renderPassCommandBuffer.end();

		m_swapchain.SubmitCommandBuffers(&renderPassCommandBuffer, 1);

		m_swapchain.PresentFrame();
	}


}
#pragma warning( pop )

#endif