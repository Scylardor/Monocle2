#pragma once
#ifdef MOE_VULKAN
#include "Graphics/Device/GraphicsDevice.h"
#include "Graphics/Resources/ResourceLayout/ResourceLayoutDescriptor.h"
#include "Graphics/Resources/ResourceSet/ResourceSetDescriptor.h"

namespace moe
{
	class VulkanDevice : public IGraphicsDevice
	{
	public:
		VulkanDevice() = default;
		~VulkanDevice() override;


		void BindSamplerToTextureUnit(int textureBindingPoint, SamplerHandle samplerHandle) override;


		void BindTextureUnit(int textureBindingPoint, TextureHandle texHandle) override;


		void BindUniformBlock(unsigned uniformBlockBinding, DeviceBufferHandle ubHandle, uint32_t bufferSize,
			uint32_t relativeOffset) override;


		void BlitFramebuffer(FramebufferHandle srcFramebuffer, FramebufferHandle destFramebuffer, TargetBuffer target,
			const Rect2Di& srcArea, const Rect2Di& destArea, SamplerFilter stretchFilter) override;


		[[nodiscard]] TextureHandle CreateCubemapTexture(const CubeMapTextureDescriptor& cubemapDesc) override;


		[[nodiscard]] TextureHandle
		CreateCubemapTexture(const CubeMapTextureFilesDescriptor& cubemapFilesDesc) override;


		[[nodiscard]] FramebufferHandle CreateFramebuffer(const FramebufferDescriptor& fbDesc) override;


		[[nodiscard]] DeviceBufferHandle CreateIndexBuffer(const void* indexData, size_t indexDataSizeBytes) override;


		[[nodiscard]] PipelineHandle CreatePipeline(PipelineDescriptor& pipelineDesc) override;


		[[nodiscard]] ResourceLayoutHandle CreateResourceLayout(const ResourceLayoutDescriptor& newDesc) override;


		[[nodiscard]] ResourceSetHandle CreateResourceSet(const ResourceSetDescriptor& newDesc) override;


		[[nodiscard]] SamplerHandle CreateSampler(const SamplerDescriptor& samplerDesc) override;


		[[nodiscard]] ShaderProgramHandle
		CreateShaderProgramFromBinary(const ShaderProgramDescriptor& shaProDesc) override;


		[[nodiscard]] ShaderProgramHandle
		CreateShaderProgramFromSource(const ShaderProgramDescriptor& shaProDesc) override;


		[[nodiscard]] DeviceBufferHandle CreateStaticVertexBuffer(const void* data, size_t dataSize) override;


		[[nodiscard]] SwapchainHandle CreateSwapChain(uint32_t renderWidth, uint32_t renderHeight,
			FramebufferAttachment wantedAttachments) override;


		[[nodiscard]] Texture2DHandle CreateTexture2D(const Texture2DDescriptor& tex2DDesc) override;


		[[nodiscard]] Texture2DHandle CreateTexture2D(const Texture2DFileDescriptor& tex2DFileDesc) override;


		[[nodiscard]] DeviceBufferHandle
		CreateUniformBuffer(const void* uniformData, size_t uniformDataSizeBytes) override;


		[[nodiscard]] VertexLayoutHandle CreateVertexLayout(InstancedVertexLayoutDescriptor desc) override;


		[[nodiscard]] VertexLayoutHandle CreateVertexLayout(const VertexLayoutDescriptor& desc) override;


		[[nodiscard]] ViewportHandle CreateViewport(const ViewportDescriptor& vpDesc) override;


		void DeleteIndexBuffer(DeviceBufferHandle idxHandle) override;


		void DeleteStaticVertexBuffer(DeviceBufferHandle vtxHandle) override;


		void Destroy() override;


		void DestroyTexture2D(Texture2DHandle textureHandle) override;


		void DrawInstancedMesh(VertexLayoutHandle vtxLayoutHandle, DeviceBufferHandle vtxBufHandle, size_t numVertices,
			DeviceBufferHandle idxBufHandle, size_t numIndices, DeviceBufferHandle instancingBuffer,
			uint32_t instancesAmount) override;


		void DrawVertexBuffer(VertexLayoutHandle vtxLayoutHandle, DeviceBufferHandle vtxBufHandle, size_t numVertices,
			DeviceBufferHandle idxBufHandle, size_t numIndices) override;


		void GenerateTextureMipmaps(TextureHandle texHandle) override;


		[[nodiscard]] const ResourceLayoutDescriptor& GetResourceLayoutDescriptor(
			ResourceLayoutHandle layoutHandle) const override;


		[[nodiscard]] const ResourceSetDescriptor& GetResourceSetDescriptor(ResourceSetHandle setHandle) const override;


		[[nodiscard]] uint32_t GetShaderProgramUniformBlockSize(ShaderProgramHandle shaderHandle,
			const std::string& uniformBlockName) override;


		[[nodiscard]] const VertexLayout* GetVertexLayout(VertexLayoutHandle handle) const override;


		void Initialize() override;


		[[nodiscard]] bool IsPartOfUniformBlock(ShaderProgramHandle shaderHandle, const std::string& uniformBlockName,
			const std::string& uniformMemberName) const override;


		[[nodiscard]] AFramebuffer* MutFramebuffer(FramebufferHandle fbHandle) override;


		bool RemoveShaderProgram(ShaderProgramHandle programHandle) override;


		void SetPipeline(PipelineHandle pipeHandle) override;


		void UpdateBuffer(DeviceBufferHandle bufferHandle, const void* data, size_t dataSize) const override;


		void UpdateResourceSetDescriptor(ResourceSetHandle rscSetHandle, uint32_t resourceIdx,
			ResourceHandle newResource) override;


		void UpdateUniformBlockVariable(ShaderProgramHandle program, DeviceBufferHandle targetBlockBuffer,
			const std::string& variableName, const void* data, size_t dataSizeBytes) override;


		void UseViewport(ViewportHandle vpHandle) override;


	private:
		// TODO: TO remove, these two are needed to compile but useless.
		ResourceLayoutDescriptor	dummyLayoutDesc{0};
		ResourceSetDescriptor		dummySetDesc;
	};
}

#endif