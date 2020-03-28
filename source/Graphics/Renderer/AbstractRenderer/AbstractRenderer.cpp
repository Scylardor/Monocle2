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


	std::optional<ShaderProgramDescriptor> AbstractRenderer::BuildProgramDescriptorFromFileList(const ShaderFileList& fileList)
	{
		ShaderProgramDescriptor programDesc(fileList.Size());

		for (auto stageFilePair : fileList)
		{
			auto shaderCodeOpt = moe::ReadFile(stageFilePair.second);
			if (shaderCodeOpt.has_value())
			{
				programDesc.m_modules.PushBack({ stageFilePair.first, shaderCodeOpt.value() });
			}
			else
			{
				MOE_ERROR(ChanGraphics, "Graphics Renderer unable to read shader %s module file %s.", stageFilePair.first._to_string(), stageFilePair.second.data());
				return std::nullopt;
			}
		}

		return { programDesc };
	}
}
