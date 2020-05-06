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


	CameraHandle AbstractRenderer::CreateCamera(const OrthographicCameraDesc& orthoDesc, const ViewportDescriptor& vpDesc)
	{
		ViewportHandle vpHandle = MutDevice().CreateViewport(vpDesc);
		return CreateCamera(orthoDesc, vpHandle);
	}


	CameraHandle AbstractRenderer::CreateCamera(const PerspectiveCameraDesc& perspDesc, const ViewportDescriptor& vpDesc)
	{
		ViewportHandle vpHandle = MutDevice().CreateViewport(vpDesc);
		return CreateCamera(perspDesc, vpHandle);
	}


	CameraHandle AbstractRenderer::CreateCamera(const OrthographicCameraDesc& orthoDesc, ViewportHandle vpHandle)
	{
		return m_cameraManager.AddCamera(vpHandle, orthoDesc);
	}


	CameraHandle AbstractRenderer::CreateCamera(const PerspectiveCameraDesc& perspDesc, ViewportHandle vpHandle)
	{
		return m_cameraManager.AddCamera(vpHandle, perspDesc);
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
