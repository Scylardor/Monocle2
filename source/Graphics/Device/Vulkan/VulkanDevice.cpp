#ifdef MOE_VULKAN

#include "VulkanDevice.h"

// This is placeholder, so I disable the "unreferenced formal parameter" warning.
#pragma warning( push )
#pragma warning( disable: 4100 )
namespace moe
{
	VulkanDevice::~VulkanDevice()
	{
	}

	void VulkanDevice::BindSamplerToTextureUnit(int textureBindingPoint, SamplerHandle samplerHandle)
	{
	}

	void VulkanDevice::BindTextureUnit(int textureBindingPoint, TextureHandle texHandle)
	{
	}

	void VulkanDevice::BindUniformBlock(unsigned uniformBlockBinding, DeviceBufferHandle ubHandle, uint32_t bufferSize,
		uint32_t relativeOffset)
	{
	}

	void VulkanDevice::BlitFramebuffer(FramebufferHandle srcFramebuffer, FramebufferHandle destFramebuffer,
		TargetBuffer target, const Rect2Di& srcArea, const Rect2Di& destArea, SamplerFilter stretchFilter)
	{
	}

	TextureHandle VulkanDevice::CreateCubemapTexture(const CubeMapTextureDescriptor& cubemapDesc)
	{
		return TextureHandle::Null();
	}

	TextureHandle VulkanDevice::CreateCubemapTexture(const CubeMapTextureFilesDescriptor& cubemapFilesDesc)
	{
		return TextureHandle::Null();
	}

	FramebufferHandle VulkanDevice::CreateFramebuffer(const FramebufferDescriptor& fbDesc)
	{
		return FramebufferHandle::Null();
	}

	DeviceBufferHandle VulkanDevice::CreateIndexBuffer(const void* indexData, size_t indexDataSizeBytes)
	{
		return DeviceBufferHandle::Null();
	}

	PipelineHandle VulkanDevice::CreatePipeline(PipelineDescriptor& pipelineDesc)
	{
		return PipelineHandle::Null();
	}

	ResourceLayoutHandle VulkanDevice::CreateResourceLayout(const ResourceLayoutDescriptor& newDesc)
	{
		return ResourceLayoutHandle::Null();
	}

	ResourceSetHandle VulkanDevice::CreateResourceSet(const ResourceSetDescriptor& newDesc)
	{
		return ResourceSetHandle::Null();
	}

	SamplerHandle VulkanDevice::CreateSampler(const SamplerDescriptor& samplerDesc)
	{
		return SamplerHandle::Null();
	}

	ShaderProgramHandle VulkanDevice::CreateShaderProgramFromBinary(const ShaderProgramDescriptor& shaProDesc)
	{
		return ShaderProgramHandle::Null();
	}

	ShaderProgramHandle VulkanDevice::CreateShaderProgramFromSource(const ShaderProgramDescriptor& shaProDesc)
	{
		return ShaderProgramHandle::Null();
	}

	DeviceBufferHandle VulkanDevice::CreateStaticVertexBuffer(const void* data, size_t dataSize)
	{
		return DeviceBufferHandle::Null();
	}

	SwapchainHandle VulkanDevice::CreateSwapChain(uint32_t renderWidth, uint32_t renderHeight,
		FramebufferAttachment wantedAttachments)
	{
		return SwapchainHandle::Null();
	}

	Texture2DHandle VulkanDevice::CreateTexture2D(const Texture2DDescriptor& tex2DDesc)
	{
		return Texture2DHandle::Null();
	}

	Texture2DHandle VulkanDevice::CreateTexture2D(const Texture2DFileDescriptor& tex2DFileDesc)
	{
		return Texture2DHandle::Null();

	}

	DeviceBufferHandle VulkanDevice::CreateUniformBuffer(const void* uniformData, size_t uniformDataSizeBytes)
	{
		return DeviceBufferHandle::Null();
	}

	VertexLayoutHandle VulkanDevice::CreateVertexLayout(InstancedVertexLayoutDescriptor desc)
	{
		return VertexLayoutHandle::Null();
	}

	VertexLayoutHandle VulkanDevice::CreateVertexLayout(const VertexLayoutDescriptor& desc)
	{
		return VertexLayoutHandle::Null();
	}

	ViewportHandle VulkanDevice::CreateViewport(const ViewportDescriptor& vpDesc)
	{
		return ViewportHandle::Null();
	}

	void VulkanDevice::DeleteIndexBuffer(DeviceBufferHandle idxHandle)
	{
	}

	void VulkanDevice::DeleteStaticVertexBuffer(DeviceBufferHandle vtxHandle)
	{
	}

	void VulkanDevice::Destroy()
	{
	}

	void VulkanDevice::DestroyTexture2D(Texture2DHandle textureHandle)
	{
	}

	void VulkanDevice::DrawInstancedMesh(VertexLayoutHandle vtxLayoutHandle, DeviceBufferHandle vtxBufHandle,
		size_t numVertices, DeviceBufferHandle idxBufHandle, size_t numIndices, DeviceBufferHandle instancingBuffer,
		uint32_t instancesAmount)
	{
	}

	void VulkanDevice::DrawVertexBuffer(VertexLayoutHandle vtxLayoutHandle, DeviceBufferHandle vtxBufHandle,
		size_t numVertices, DeviceBufferHandle idxBufHandle, size_t numIndices)
	{
	}

	void VulkanDevice::GenerateTextureMipmaps(TextureHandle texHandle)
	{
	}

	const ResourceLayoutDescriptor& VulkanDevice::GetResourceLayoutDescriptor(ResourceLayoutHandle layoutHandle) const
	{
		return dummyLayoutDesc;
	}

	const ResourceSetDescriptor& VulkanDevice::GetResourceSetDescriptor(ResourceSetHandle setHandle) const
	{
		return dummySetDesc;
	}

	uint32_t VulkanDevice::GetShaderProgramUniformBlockSize(ShaderProgramHandle shaderHandle,
		const std::string& uniformBlockName)
	{
		return 0;
	}

	const VertexLayout* VulkanDevice::GetVertexLayout(VertexLayoutHandle handle) const
	{
		return nullptr;
	}

	void VulkanDevice::Initialize()
	{
	}

	bool VulkanDevice::IsPartOfUniformBlock(ShaderProgramHandle shaderHandle, const std::string& uniformBlockName,
		const std::string& uniformMemberName) const
	{
		return false;
	}

	AFramebuffer* VulkanDevice::MutFramebuffer(FramebufferHandle fbHandle)
	{
		return nullptr;
	}

	bool VulkanDevice::RemoveShaderProgram(ShaderProgramHandle programHandle)
	{
		return false;
	}

	void VulkanDevice::SetPipeline(PipelineHandle pipeHandle)
	{
	}

	void VulkanDevice::UpdateBuffer(DeviceBufferHandle bufferHandle, const void* data, size_t dataSize) const
	{
	}

	void VulkanDevice::UpdateResourceSetDescriptor(ResourceSetHandle rscSetHandle, uint32_t resourceIdx,
		ResourceHandle newResource)
	{
	}

	void VulkanDevice::UpdateUniformBlockVariable(ShaderProgramHandle program, DeviceBufferHandle targetBlockBuffer,
		const std::string& variableName, const void* data, size_t dataSizeBytes)
	{
	}

	void VulkanDevice::UseViewport(ViewportHandle vpHandle)
	{
	}
}
#pragma warning( pop )

#endif