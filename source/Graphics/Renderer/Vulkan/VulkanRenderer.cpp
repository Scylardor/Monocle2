
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
	vk::VertexInputBindingDescription GetVertex_PosColorUVBindingDescription()
	{
		vk::VertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex_PosColorUV);
		bindingDescription.inputRate = vk::VertexInputRate::eVertex;

		return bindingDescription;
	}

	std::array<vk::VertexInputAttributeDescription, 3> GetVertex_PosColorUVAttributeDescriptions()
	{
		std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
		attributeDescriptions[0].offset = offsetof(Vertex_PosColorUV, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
		attributeDescriptions[1].offset = offsetof(Vertex_PosColorUV, Color);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 2;
		attributeDescriptions[1].format = vk::Format::eR32G32Sfloat;
		attributeDescriptions[1].offset = offsetof(Vertex_PosColorUV, Texture_UV0);

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
			.AddVertexAttribute(0, offsetof(Vertex_PosColorUV, Position), sizeof(Vertex_PosColorUV::Position), vk::Format::eR32G32B32Sfloat)
			.AddVertexAttribute(1, offsetof(Vertex_PosColorUV, Color), sizeof(Vertex_PosColorUV::Color), vk::Format::eR32G32B32Sfloat)
			.AddVertexAttribute(2, offsetof(Vertex_PosColorUV, Texture_UV0), sizeof(Vertex_PosColorUV::Texture_UV0), vk::Format::eR32G32Sfloat)
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


		m_defaultMaterial = m_resourceManager->Load<MaterialModulesResource>(HashString("DefaultMaterial"), [&]()
		{
				return std::make_unique<VulkanMaterial>();
		});

		auto texRef = m_resourceManager->FindExisting<TextureResource>(HashString("StatueTex"));


		m_defaultMaterial.As<VulkanMaterial>().Initialize(*m_graphicsDevice, std::move(pipelineRef))
			.BindTexture(0, 1, texRef.As<VulkanTexture>())
			.UpdateAllDescriptorSets();

		auto& phongMapsModule = m_defaultMaterial->EmplaceModule<PhongReflectivityMapsMaterialModule>(0);
		phongMapsModule.Set(PhongMap::Diffuse, texRef);

		m_defaultMaterial->UpdateResources(0);
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



	void VulkanRenderer::RenderFrame(RenderScene& renderedScene)
	{
		m_swapchain.PrepareNewFrame();

		renderedScene.Update(m_swapchain.GetFrameIndex());

		auto& thisFrameCommandPool = m_commandPools[m_swapchain.GetFrameIndex()];
		thisFrameCommandPool.Reset(*m_graphicsDevice);

		auto optCb = thisFrameCommandPool.TryGrabCommandBuffer();
		MOE_ASSERT(optCb);
		vk::CommandBuffer renderPassCommandBuffer = optCb.value();

		renderPassCommandBuffer.begin(vk::CommandBufferBeginInfo{});

		auto& rp = m_frameGraph.MutMainRenderPass();

		rp.Begin(renderPassCommandBuffer, m_swapchain.GetImageInFlightIndex());

		renderedScene.CameraSystem().ForeachCamera([&](CameraDesc& camera)
			{
				RenderSceneWithCamera(renderedScene, renderPassCommandBuffer, camera);
			});

		rp.End(renderPassCommandBuffer);

		renderPassCommandBuffer.end();

		m_swapchain.SubmitCommandBuffers(&renderPassCommandBuffer, 1);

		m_swapchain.PresentFrame();
	}


	void VulkanRenderer::RenderSceneWithCamera(RenderScene& renderScene, vk::CommandBuffer commandBuffer, CameraDesc& camera)
	{
		const auto& camViewportScissor = renderScene.GetCameraSystem().GetCameraViewportScissor(camera.ID);
		// get the view proj in case we need to recompute drawable's MVP
		const Mat4& cameraViewProj = renderScene.GetCameraSystem().GetViewProjection(camera.ID);

		commandBuffer.setViewport(0, 1, &camViewportScissor.first);
		commandBuffer.setScissor(0, 1, &camViewportScissor.second);

		uint32_t lastMatID = ~0u;
		const VulkanMaterial& defaultMaterial = m_defaultMaterial.As<VulkanMaterial>();
		const VulkanMaterial* currentMaterial = nullptr;

		for (auto& drawable : renderScene)
		{
			auto matID = drawable.GetMaterialID();
			if (lastMatID != matID)
			{
				if (matID != 0)
				{
					const auto& drawableMaterial = m_resourceManager->GetResourceAs<VulkanMaterial>(matID);
					currentMaterial = &drawableMaterial;
					drawableMaterial.Bind(commandBuffer);
				}
				else
				{
					currentMaterial = &defaultMaterial;
				}

				currentMaterial->Bind(commandBuffer);

				lastMatID = matID;
			}

			MOE_ASSERT(currentMaterial != nullptr);
			if (camera.UpdatedSinceLastRender || drawable.WasUpdatedSinceLastRender())
			{
				drawable.SetMVP(cameraViewProj * drawable.GetModelMatrix());
			}

			drawable.BindPerObjectResources(currentMaterial->GetPipeline(), commandBuffer);

			const auto& drawableMesh = m_resourceManager->GetResourceAs< VulkanMesh >(drawable.GetMeshID());

			drawableMesh.Draw(commandBuffer);
		}

		camera.UpdatedSinceLastRender = false;
	}
}
#pragma warning( pop )

#endif