
#ifdef MOE_VULKAN

#include "Core/Misc/moeFile.h"
#include "VulkanRenderer.h"
#include "Graphics/Vulkan/Surface/VulkanSurfaceProvider.h"


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

		CreateMainMaterial();

		CreateCommandPools();

		return ok;
	}


	void VulkanRenderer::CreateMainMaterial()
	{
		// First create our texture
		VulkanTextureBuilder texBuilder{};
		m_materialTexture = VulkanTexture::Create2DFromFile(*m_graphicsDevice, "Sandbox/assets/textures/texture.jpg", texBuilder);

		VulkanShaderProgram program;

		program.AddShaderFile(*m_graphicsDevice, "source/Graphics/Resources/shaders/Vulkan/vert.spv", vk::ShaderStageFlagBits::eVertex);
		program.AddShaderFile(*m_graphicsDevice, "source/Graphics/Resources/shaders/Vulkan/frag.spv", vk::ShaderStageFlagBits::eFragment);
		program.AddPushConstant(vk::ShaderStageFlagBits::eVertex, 0, sizeof(Mat4)); // object mvp
		program.AddVertexBinding(vk::VertexInputRate::eVertex)
			.AddVertexAttribute(0, offsetof(VertexData, pos), sizeof(VertexData::pos), vk::Format::eR32G32B32Sfloat)
			.AddVertexAttribute(1, offsetof(VertexData, color), sizeof(VertexData::color), vk::Format::eR32G32B32Sfloat)
			.AddVertexAttribute(2, offsetof(VertexData, texCoord), sizeof(VertexData::texCoord), vk::Format::eR32G32Sfloat)
			.AddNewDescriptorSetLayout()
				//.AddNewDescriptorBinding(0, 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex)
				.AddNewDescriptorBinding(0, 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment)
			.Compile(*m_graphicsDevice);

		m_pipeline.SetShaderProgram(std::move(program))
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

		m_material.Initialize(*m_graphicsDevice, m_pipeline)
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

		rp.Begin(renderPassCommandBuffer);

		renderPassCommandBuffer.setViewport(0, (uint32_t)m_pipeline.Viewports().size(), m_pipeline.Viewports().data());
		renderPassCommandBuffer.setScissor(0, (uint32_t)m_pipeline.Scissors().size(), m_pipeline.Scissors().data());

		for (const auto& drawable : renderedScene)
		{
			auto matID = drawable.GetMaterialID();
			if (matID == 0)
				m_material.Bind(renderPassCommandBuffer);

			drawable.BindTransform(m_pipeline, renderPassCommandBuffer);
			const VulkanMesh& drawableMesh = m_meshStorage[drawable.GetMeshID()];
			drawableMesh.Draw(renderPassCommandBuffer);
		}
		//for (const auto& mesh : m_meshStorage)
		//{
		//	mesh.Draw(renderPassCommandBuffer);
		//}

		rp.End(renderPassCommandBuffer);

		renderPassCommandBuffer.end();

		m_swapchain.SubmitCommandBuffers(&renderPassCommandBuffer, 1);

		m_swapchain.PresentFrame();
	}


	uint32_t VulkanRenderer::EmplaceMesh(size_t vertexSize, size_t numVertices, const void* vertexData, size_t numIndices, const void* indexData, vk::IndexType indexType)
	{
		auto ID = m_meshStorage.size();
		m_meshStorage.emplace_back(*m_graphicsDevice, vertexSize, numVertices, vertexData, numIndices, indexData, indexType);
		return (uint32_t) ID;
	}
}
#pragma warning( pop )

#endif