
#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Renderer/AbstractRenderer/AbstractRenderer.h"

#include "Graphics/RHI/Vulkan/VulkanRHI.h"

#include "Graphics/Vulkan/Swapchain/VulkanSwapchain.h"
#include "Graphics/Vulkan/Pipeline/VulkanPipeline.h"

#include "Graphics/Vulkan/Material/VulkanMaterial.h"
#include "Graphics/Vulkan/RenderScene/RenderScene.h"

#include "Graphics/Device/Vulkan/VulkanDevice.h"
#include "Graphics/Vulkan/FrameGraph/VulkanFrameGraph.h"
#include "Graphics/Vulkan/Mesh/VulkanMesh.h"

#include "Core/Containers/AssetRegistry/ObjectRegistry.h"

namespace moe
{
	class IVulkanSurfaceProvider;

	vk::VertexInputBindingDescription GetBasicVertexBindingDescription();


	std::array<vk::VertexInputAttributeDescription, 3> GetBasicVertexAttributeDescriptions();


	class VulkanRenderer : public AbstractRenderer
	{
	public:

		VulkanRenderer();

		bool Initialize(GraphicsContextSetup setupFunction) override;

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

		void	CreateMainMaterial();


		vk::UniqueShaderModule	CreateShaderModule(std::string_view bytecode);

		void			CreateCommandPools();


		VkInstance	GetVkInstance()
		{
			return m_rhi.GetInstance();
		}


		void	RenderFrame(const RenderScene& renderedScene);


		auto	GetMaxFramesInFlight() const
		{
			return m_swapchain.GetMaxFramesInFlight();
		}


		MyVkDevice&	GraphicsDevice()
		{
			MOE_ASSERT(m_graphicsDevice);
			return *m_graphicsDevice;
		}

		void	AttachResourceManager(ResourceManager& manager)
		{
			m_resourceManager = &manager;
		}

	protected:

		[[nodiscard]] const IGraphicsDevice& GetDevice() const override
		{ return m_device; }

		[[nodiscard]] IGraphicsDevice& MutDevice() override
		{ return m_device; }

	private:

		VulkanDevice		m_device;

		VulkanRHI			m_rhi;

		class MyVkDevice*	m_graphicsDevice{nullptr};

		VulkanSwapchain		m_swapchain{};

		VulkanFrameGraph	m_frameGraph;

		RenderWorld		m_world;

		std::vector<VulkanCommandPool>	m_commandPools;

		Ref<MaterialResource>	m_defaultMaterial;


		ResourceManager* m_resourceManager{ nullptr };
	};


}
#endif