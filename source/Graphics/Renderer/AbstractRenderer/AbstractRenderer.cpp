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

		int iBlockBinding = 0;
		int	iTexUnit = 0;

		for (const ResourceLayoutBindingDescriptor& rscBindingDesc : rscLayoutDesc)
		{
			switch (rscBindingDesc.m_kind)
			{
			case ResourceKind::UniformBuffer:
			{
				DeviceBufferHandle ubHandle = rscSetDesc.Get<DeviceBufferHandle>(iBlockBinding);
				device.BindUniformBlock(rscBindingDesc.m_bindingPoint, ubHandle);
				iBlockBinding++;
			}
			break;
			case ResourceKind::TextureReadOnly:
			{
				Texture2DHandle tex2DHandle = rscSetDesc.Get<Texture2DHandle>(iTexUnit);
				device.BindTextureUnit(rscBindingDesc.m_bindingPoint, tex2DHandle);
				iTexUnit++;
			}

			break;
			case ResourceKind::Sampler:

				break;
			default:
				MOE_ASSERT(false);
				MOE_ERROR(ChanGraphics, "Unmanaged ResourceKind value.");
			}

			iBlockBinding++;
		}
	}


	std::optional<ShaderProgramDescriptor> AbstractRenderer::BuildProgramDescriptorFromFileList(const ShaderFileList& fileList)
	{
		ShaderProgramDescriptor programDesc(fileList.Size());

		for (const auto & [shaderStage, fileName] : fileList)
		{
			auto shaderCodeOpt = moe::ReadFile(fileName);
			if (shaderCodeOpt.has_value())
			{
				programDesc.m_modules.PushBack({ shaderStage, shaderCodeOpt.value() });
			}
			else
			{
				MOE_ERROR(ChanGraphics, "Graphics Renderer unable to read shader %d module file %s.", shaderStage, fileName.data()); // TODO use magic enums
				return std::nullopt;
			}
		}

		return { programDesc };
	}
}
