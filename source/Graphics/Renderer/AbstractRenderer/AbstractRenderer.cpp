// Monocle Game Engine source files - Alexandre Baron

#include "AbstractRenderer.h"

#include "Core/Misc/moeFile.h"

namespace moe
{
	ShaderProgramHandle AbstractRenderer::CreateShaderProgramFromSourceFiles(const ShaderFileList& fileList)
	{
		auto programDescOpt = BuildProgramDescriptorFromFileList(fileList);
		if (programDescOpt.has_value())
		{
			return CreateShaderProgramFromSource(programDescOpt.value());
		}

		return ShaderProgramHandle::Null();
	}


	ShaderProgramHandle AbstractRenderer::CreateShaderProgramFromBinaryFiles(const ShaderFileList& fileList)
	{
		auto programDescOpt = BuildProgramDescriptorFromFileList(fileList);
		if (programDescOpt.has_value())
		{
			return CreateShaderProgramFromBinary(programDescOpt.value());
		}

		return ShaderProgramHandle::Null();
	}


	void AbstractRenderer::UseResourceSet(const ResourceSetHandle rscSetHandle)
	{
		if (rscSetHandle.IsNull())
			return;

		IGraphicsDevice& device = MutGraphicsDevice();

		const auto& rscSetDesc = device.GetResourceSetDescriptor(rscSetHandle);

		const auto& rscLayoutDesc = device.GetResourceLayoutDescriptor(rscSetDesc.GetResourceLayoutHandle());

		int iBinding = 0;

		SamplerHandle lastSamplerUsed{0};

		for (const ResourceLayoutBindingDescriptor& rscBindingDesc : rscLayoutDesc)
		{
			switch (rscBindingDesc.m_kind)
			{
			case ResourceKind::UniformBuffer:
			{
				DeviceBufferHandle ubHandle = rscSetDesc.Get<DeviceBufferHandle>(iBinding);
				device.BindUniformBlock(rscBindingDesc.m_bindingPoint, ubHandle);
			}
			break;
			case ResourceKind::TextureReadOnly:
			{
				TextureHandle texHandle = rscSetDesc.Get<TextureHandle>(iBinding);
				device.BindTextureUnit(rscBindingDesc.m_bindingPoint, texHandle);

				// The way Samplers work in Monocle is that the last sampler used automatically binds itself to the next textures.
				if (lastSamplerUsed.IsNotNull())
				{
					device.BindSamplerToTextureUnit(rscBindingDesc.m_bindingPoint, lastSamplerUsed);
				}
			}
			break;
			case ResourceKind::Sampler:
				lastSamplerUsed = rscSetDesc.Get<SamplerHandle>(iBinding);
			break;
			default:
				MOE_ASSERT(false);
				MOE_ERROR(ChanGraphics, "Unmanaged ResourceKind value.");
			}

			iBinding++;
		}
	}


	std::optional<ShaderProgramDescriptor> AbstractRenderer::BuildProgramDescriptorFromFileList(const ShaderFileList& fileList)
	{
		ShaderProgramDescriptor programDesc(fileList.Size());

		for (const auto & [shaderStage, fileName] : fileList)
		{
			auto shaderCodeOpt = moe::ReadFile(fileName, FileMode::Binary);
			if (shaderCodeOpt.has_value())
			{
				programDesc.m_modules.PushBack({ shaderStage, shaderCodeOpt.value() });
			}
			else
			{
				MOE_ERROR(ChanGraphics, "Graphics Renderer unable to read shader %d module file %s.", shaderStage, fileName.data()); // TODO use magic enums
				MOE_DEBUG_ASSERT(false);
				return std::nullopt;
			}
		}

		return { programDesc };
	}
}
