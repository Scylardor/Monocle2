
#ifdef MOE_VULKAN

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
		vkDeviceWaitIdle(*(*m_graphicsDevice));
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

		m_commandPools.reserve(m_swapchain.GetMaxFramesInFlight());
		for (auto iFrame = 0u; iFrame < m_swapchain.GetMaxFramesInFlight(); iFrame++)
		{
			vk::CommandPoolCreateInfo poolCreateInfo{ vk::CommandPoolCreateFlags(), m_graphicsDevice->GraphicsQueueIdx() };
			vk::CommandBufferAllocateInfo cbCreateInfo{ vk::CommandPool(), vk::CommandBufferLevel::ePrimary, 1 };
			m_commandPools.emplace_back(*m_graphicsDevice, poolCreateInfo, cbCreateInfo);
		}


		return ok;
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
		rp.End(renderPassCommandBuffer);

		renderPassCommandBuffer.end();

		m_swapchain.SubmitCommandBuffers(&renderPassCommandBuffer, 1);

		m_swapchain.PresentFrame();
	}
}
#pragma warning( pop )

#endif