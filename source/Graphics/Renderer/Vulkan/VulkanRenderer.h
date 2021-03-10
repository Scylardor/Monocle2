
#pragma once

#ifdef MOE_VULKAN


#include "Graphics/Renderer/AbstractRenderer/AbstractRenderer.h"

#include "Graphics/RHI/Vulkan/VulkanRHI.h"

#include "Graphics/Device/Vulkan/VulkanDevice.h"


#include "Graphics/Vulkan/Swapchain/VulkanSwapchain.h"

namespace moe
{
	class IVulkanSurfaceProvider;

	class VulkanRenderer : public AbstractRenderer
	{
	public:

		VulkanRenderer();

		bool Initialize(IGraphicsRenderer::GraphicsContextSetup setupFunction) override;

		void Shutdown() override;

		[[nodiscard]] ShaderProgramHandle
		CreateShaderProgramFromSource(const ShaderProgramDescriptor& shaProDesc) override;

		[[nodiscard]] ShaderProgramHandle
		CreateShaderProgramFromBinary(const ShaderProgramDescriptor& shaProDesc) override;

		bool RemoveShaderProgram(ShaderProgramHandle programHandle) override;


		void UseShaderProgram(ShaderProgramHandle shaderProgram) override;

		[[nodiscard]] VertexLayoutHandle CreateVertexLayout(const VertexLayoutDescriptor& vertexLayoutDesc) override;

		[[nodiscard]] VertexLayoutHandle CreateInstancedVertexLayout(
			const InstancedVertexLayoutDescriptor& vertexLayoutDesc) override;

		[[nodiscard]] DeviceBufferHandle CreateUniformBuffer(const void* data, uint32_t dataSizeBytes) override;

		[[nodiscard]] ResourceLayoutHandle CreateResourceLayout(const ResourceLayoutDescriptor& desc) override;

		[[nodiscard]] ResourceSetHandle CreateResourceSet(const ResourceSetDescriptor& desc) override;

		[[nodiscard]] const IGraphicsDevice& GetGraphicsDevice() const override;

		[[nodiscard]] IGraphicsDevice& MutGraphicsDevice() override;

		[[nodiscard]] DeviceBufferHandle AllocateObjectMemory(const uint32_t size) override;

		void CopyObjectMemory(DeviceBufferHandle from, uint32_t fromSizeBytes, DeviceBufferHandle to) override;

		void ReleaseObjectMemory(DeviceBufferHandle freedHandle) override;

		void UpdateSubBufferRange(DeviceBufferHandle handle, uint32_t offset, void* data, uint32_t dataSize) override;

		[[nodiscard]] GraphicObjectData ReallocObjectUniformGraphicData(const GraphicObjectData& oldData,
			uint32_t newNeededSize) override;

		void Clear(const ColorRGBAf& clearColor) override;
		void ClearDepth() override;

		void UseMaterial(ShaderProgramHandle progHandle, ResourceSetHandle rscSetHandle) override;
		void UseMaterial(Material* material) override;

		void UseMaterialInstance(const MaterialInstance* material) override;

		void UseMaterialPerObject(Material* material, AGraphicObject& object) override;
		void BindFramebuffer(FramebufferHandle fbHandle) override;

		void UnbindFramebuffer(FramebufferHandle fbHandle) override;

		~VulkanRenderer() override = default;

		[[nodiscard]] RenderWorld& CreateRenderWorld() override;

		void UseResourceSet(const ResourceSetHandle rscSetHandle) override;

		bool	Initialize(VulkanInstance::CreationParams&& instanceParams, IVulkanSurfaceProvider& surfaceProvider);

		VkInstance	GetVkInstance()
		{
			return m_rhi.GetInstance();
		}

	protected:

		[[nodiscard]] const IGraphicsDevice& GetDevice() const override
		{ return m_device; }

		[[nodiscard]] IGraphicsDevice& MutDevice() override
		{ return m_device; }

	private:

		VulkanDevice		m_device;

		VulkanRHI			m_rhi;

		VulkanSwapchain		m_swapchain;

		RenderWorld		m_world;
	};


}
#endif