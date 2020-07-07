// Monocle Game Engine source files - Alexandre Baron

#ifdef MOE_OPENGL

#include "OpenGLRenderer.h"

#include <glad/glad.h>

#include "Graphics/Material/MaterialInstance.h"

namespace moe
{
	bool OpenGLRenderer::Initialize(IGraphicsRenderer::GraphicsContextSetup setupFunction)
	{
		// Very important to do this call before any OpenGL function is used !
		if (false == gladLoadGLLoader((GLADloadproc)setupFunction))
		{
			return false;
		}

		// Activate OpenGL debug messages callback
		GLint flags = 0;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLRenderer::OpenGLDebugMessageRoutine, this);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE); // we want to log all errors
		}

		// Now we can start initialization
		m_device.Initialize();

		/* TODO: remove or put somewhere else */
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glEnable(GL_MULTISAMPLE);

		return true;
	}


	void OpenGLRenderer::Shutdown()
	{
		m_device.Destroy();
	}


	DeviceBufferHandle OpenGLRenderer::CreateUniformBuffer(const void* data, uint32_t dataSizeBytes)
	{
		return m_device.CreateUniformBuffer(data, dataSizeBytes);
	}


	UniformResourceKind OpenGLRenderer::GetUniformResourceKind(const std::string& uniformBlockName) const
	{
		const auto kindIt = m_uniformResourceKinds.Find(uniformBlockName);
		if (kindIt != m_uniformResourceKinds.End())
		{
			return kindIt->second;
		}

		MOE_ASSERT(false); // should not happen...
		return UniformResourceKind::None;
	}


	void OpenGLRenderer::SetUniformResourceKind(const std::string& uniformBlockName, UniformResourceKind resourceKind)
	{
		m_uniformResourceKinds.Insert({uniformBlockName, resourceKind});
	}


	void OpenGLRenderer::Clear(const ColorRGBAf& clearColor)
	{
		glClearColor(clearColor.R(), clearColor.G(), clearColor.B(), clearColor.A());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}


	void OpenGLRenderer::ClearDepth()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}


	void OpenGLRenderer::UseMaterial(ShaderProgramHandle progHandle, ResourceSetHandle rscSetHandle)
	{
		const GLuint shaderProgramID = m_device.UseShaderProgram(progHandle);

		if (rscSetHandle.IsNull())
			return;

		const auto& rscSetDesc = m_device.GetResourceSetDescriptor(rscSetHandle);

		const auto& rscLayoutDesc = m_device.GetResourceLayoutDescriptor(rscSetDesc.GetResourceLayoutHandle());

		int iBinding = 0;

		int	uniformBlockBinding = 0;
		int	textureUnitIndex = 0;
		for (const ResourceLayoutBindingDescriptor& rscBindingDesc : rscLayoutDesc)
		{
			switch (rscBindingDesc.m_kind)
			{
			case ResourceKind::UniformBuffer:
				{
					DeviceBufferHandle ubHandle = rscSetDesc.Get<DeviceBufferHandle>(iBinding);
					m_device.BindProgramUniformBlock(shaderProgramID, rscBindingDesc.m_name.c_str(), uniformBlockBinding, ubHandle);
					uniformBlockBinding++;
				}
				break;
			case ResourceKind::TextureReadOnly:
				{
					TextureHandle texHandle = rscSetDesc.Get<TextureHandle>(iBinding);
					m_device.BindTextureUnitToProgramUniform(shaderProgramID, textureUnitIndex, texHandle, rscBindingDesc.m_name.c_str());
					textureUnitIndex++;
				}

				break;
			case ResourceKind::Sampler:

				break;
			default:
				MOE_ASSERT(false);
				MOE_ERROR(ChanGraphics, "Unmanaged ResourceKind value.");
			}

			iBinding++;
		}
	}


	void OpenGLRenderer::UseMaterial(Material* material)
	{
		if (material == nullptr)
			return;

		material->ResetFrameUniformBlockCounter();

		// Process per-material resource sets.
		const GLuint shaderProgramID = m_device.UseShaderProgram(material->GetShaderProgramHandle());

		int	uniformBlockBinding = 0;

		for (ResourceSetHandle rscSetHandle : material->GetPerMaterialResourceSets())
		{
			if (rscSetHandle.IsNull())
				return;

			const auto& rscSetDesc = m_device.GetResourceSetDescriptor(rscSetHandle);

			const auto& rscLayoutDesc = m_device.GetResourceLayoutDescriptor(rscSetDesc.GetResourceLayoutHandle());

			int iBinding = 0;


			int	textureUnitIndex = 0;
			for (const ResourceLayoutBindingDescriptor& rscBindingDesc : rscLayoutDesc)
			{
				switch (rscBindingDesc.m_kind)
				{
				case ResourceKind::UniformBuffer:
				{
					DeviceBufferHandle ubHandle = rscSetDesc.Get<DeviceBufferHandle>(iBinding);
					m_device.BindProgramUniformBlock(shaderProgramID, rscBindingDesc.m_name.c_str(), uniformBlockBinding, ubHandle);
					uniformBlockBinding++;
				}
				break;
				case ResourceKind::TextureReadOnly:
				{
					TextureHandle texHandle = rscSetDesc.Get<TextureHandle>(iBinding);
					m_device.BindTextureUnitToProgramUniform(shaderProgramID, textureUnitIndex, texHandle, rscBindingDesc.m_name.c_str());
					textureUnitIndex++;
				}

				break;
				case ResourceKind::Sampler:

					break;
				default:
					MOE_ASSERT(false);
					MOE_ERROR(ChanGraphics, "Unmanaged ResourceKind value.");
				}

				iBinding++;
			}
		}

		material->SetFrameUniformBlockCounter(uniformBlockBinding);
	}


	void OpenGLRenderer::UseMaterialPerObject(Material* material, AGraphicObject& object)
	{
		if (material == nullptr)
			return;

		// As we may have already set up per-material uniform blocks, start the uniform block binding counting at the block counter for this frame.
		int	uniformBlockBinding = material->GetFrameUniformBlockCounter();

		const GLuint shaderProgramID = m_device.GetShaderProgramID(material->GetShaderProgramHandle());

		for (ResourceSetHandle rscSetHandle : material->GetPerObjectResourceSets())
		{
			if (rscSetHandle.IsNull())
				return;

			const auto& rscSetDesc = m_device.GetResourceSetDescriptor(rscSetHandle);

			const auto& rscLayoutDesc = m_device.GetResourceLayoutDescriptor(rscSetDesc.GetResourceLayoutHandle());

			int iBinding = 0;

			int	textureUnitIndex = 0;
			for (const ResourceLayoutBindingDescriptor& rscBindingDesc : rscLayoutDesc)
			{
				switch (rscBindingDesc.m_kind)
				{
				case ResourceKind::UniformBuffer:
				{
					DeviceBufferHandle ubHandle = rscSetDesc.Get<DeviceBufferHandle>(iBinding);

					if (rscBindingDesc.m_name == "ObjectMatrices")
					{
						Material::UpdateObjectMatrices(object, ubHandle);
					}

					m_device.BindProgramUniformBlock(shaderProgramID, rscBindingDesc.m_name.c_str(), uniformBlockBinding, ubHandle);

					uniformBlockBinding++;
				}
				break;
				case ResourceKind::TextureReadOnly:
				{
					TextureHandle texHandle = rscSetDesc.Get<TextureHandle>(textureUnitIndex);
					m_device.BindTextureUnitToProgramUniform(shaderProgramID, textureUnitIndex, texHandle, rscBindingDesc.m_name.c_str());
					textureUnitIndex++;
				}

				break;
				case ResourceKind::Sampler:

					break;
				default:
					MOE_ASSERT(false);
					MOE_ERROR(ChanGraphics, "Unmanaged ResourceKind value.");
				}

				iBinding++;
			}
		}
	}


	void OpenGLRenderer::UseMaterialInstance(const MaterialInstance* material)
	{
		if (material == nullptr)
			return;

		m_device.UseShaderProgram(material->GetShaderHandle());

		// Process the material resource set.

		ResourceSetHandle rscSetHandle = material->GetResourceSetHandle();

		UseResourceSet(rscSetHandle);
	}


	RenderWorld& OpenGLRenderer::CreateRenderWorld()
	{
		RenderWorld& newRw = AbstractRenderer::CreateRenderWorld();

		// Allocate memory for this render world
		m_renderWorldMemory.ReservePoolMemory(GL_DYNAMIC_STORAGE_BIT);

		return newRw;
	}


	DeviceBufferHandle OpenGLRenderer::AllocateObjectMemory(const uint32_t size)
	{
		uint32_t offset = m_renderWorldMemory.Allocate(nullptr, size);

		DeviceBufferHandle bufferHandle = m_device.EncodeBufferHandle(m_renderWorldMemory.GetBufferHandle(), offset);

		return bufferHandle;
	}


	void OpenGLRenderer::CopyObjectMemory(DeviceBufferHandle from, uint32_t fromSizeBytes, DeviceBufferHandle to)
	{
		auto[frombuf, fromOffset] = m_device.DecodeBufferHandle(from);
		auto[tobuf, toOffset] = m_device.DecodeBufferHandle(to);
		m_renderWorldMemory.Copy(fromOffset, fromSizeBytes, toOffset);
	}


	void OpenGLRenderer::ReleaseObjectMemory(DeviceBufferHandle freedHandle)
	{
		auto [buf, bufOffset] = m_device.DecodeBufferHandle(freedHandle);
		m_renderWorldMemory.Free(bufOffset);
	}


	GraphicObjectData	OpenGLRenderer::ReallocObjectUniformGraphicData(const GraphicObjectData& oldData, uint32_t newNeededSize)
	{
		// First, get a block of the wanted size
		DeviceBufferHandle newMemHandle = AllocateObjectMemory(newNeededSize);

		// Now, copy the old region into the new region.
		// Reallocating uniform data is easy, we just extend the end of the buffer...
		CopyObjectMemory(oldData.m_objectDataHandle, oldData.TotalGraphicSize(), newMemHandle);

		// We don't need the old memory block now
		ReleaseObjectMemory(oldData.m_objectDataHandle);

		// Finally, we need to point the handles to the new data block.
		// Fortunately, OpenGL device buffer handles are just a buffer ID and an offset.
		// So we just have to update the offset...
		auto [newBuf, newBufOffset] = m_device.DecodeBufferHandle(newMemHandle);

		GraphicObjectData newData = oldData;

		auto[oldbuf, oldBufOffset] = m_device.DecodeBufferHandle(oldData.m_objectDataHandle);
		auto[vtxbuf, vtxBufOffset] = m_device.DecodeBufferHandle(oldData.m_vtxDataHandle);
		auto[idxbuf, idxBufOffset] = m_device.DecodeBufferHandle(oldData.m_idxDataHandle);
		auto[unibuf, uniBufOffset] = m_device.DecodeBufferHandle(oldData.m_uniformDataHandle);

		newData.m_objectDataHandle = newMemHandle;
		newData.m_vtxDataHandle = m_device.EncodeBufferHandle(newBuf, newBufOffset + (vtxBufOffset - oldBufOffset));
		newData.m_idxDataHandle = m_device.EncodeBufferHandle(newBuf, newBufOffset + (idxBufOffset - oldBufOffset));
		newData.m_uniformDataHandle = m_device.EncodeBufferHandle(newBuf, newBufOffset + (uniBufOffset - oldBufOffset));

		// We reallocated uniform buffer data, so only uniform data has been resized. The new uniform size is thus the difference between new and old total size.
		newData.m_uniformDataSize += (newNeededSize - newData.TotalGraphicSize());

		return newData;
	}


	void OpenGLRenderer::UpdateSubBufferRange(DeviceBufferHandle handle, uint32_t offset, void* data, uint32_t dataSize)
	{
		auto [bufID, bufOffset] = OpenGLGraphicsDevice::DecodeBufferHandle(handle);
		DeviceBufferHandle offsetHandle = OpenGLGraphicsDevice::EncodeBufferHandle(bufID, bufOffset + offset);

		MutGraphicsDevice().UpdateBuffer(offsetHandle, data, dataSize);
	}


	void OpenGLRenderer::BindFramebuffer(FramebufferHandle fbHandle)
	{
		AFramebuffer* fb = MutGraphicsDevice().MutFramebuffer(fbHandle);
		if (MOE_ASSERT(fb != nullptr))
		{
			fb->Bind();
		}
	}


	void OpenGLRenderer::UnbindFramebuffer(FramebufferHandle fbHandle)
	{
		AFramebuffer* fb = MutGraphicsDevice().MutFramebuffer(fbHandle);
		if (MOE_ASSERT(fb != nullptr))
		{
			fb->Unbind();
		}
	}


	// Logic mostly courtesy of https://learnopengl.com/In-Practice/Debugging
	void OpenGLRenderer::OpenGLDebugMessageRoutine(GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei /*length*/, const GLchar* message, const void* /*userParam*/)
	{
		// ignore non-significant error/warning codes
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
			return;

		std::string errorMessage;
		errorMessage.reserve(128);

		errorMessage = "OpenGL error (ID %u) : %s\n";

		switch (source)
		{
			case GL_DEBUG_SOURCE_API:             errorMessage += "Source: API\n"; break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   errorMessage += "Source: Window System\n"; break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: errorMessage += "Source: Shader Compiler\n"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:     errorMessage += "Source: Third Party\n"; break;
			case GL_DEBUG_SOURCE_APPLICATION:     errorMessage += "Source: Application\n"; break;
			case GL_DEBUG_SOURCE_OTHER:           errorMessage += "Source: Other\n"; break;
		}

		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR:               errorMessage += "Type: Error\n"; break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: errorMessage += "Type: Deprecated Behaviour\n"; break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  errorMessage += "Type: Undefined Behaviour\n"; break;
			case GL_DEBUG_TYPE_PORTABILITY:         errorMessage += "Type: Portability\n"; break;
			case GL_DEBUG_TYPE_PERFORMANCE:         errorMessage += "Type: Performance\n"; break;
			case GL_DEBUG_TYPE_MARKER:              errorMessage += "Type: Marker\n"; break;
			case GL_DEBUG_TYPE_PUSH_GROUP:          errorMessage += "Type: Push Group\n"; break;
			case GL_DEBUG_TYPE_POP_GROUP:           errorMessage += "Type: Pop Group\n"; break;
			case GL_DEBUG_TYPE_OTHER:               errorMessage += "Type: Other\n"; break;
		}

		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:         errorMessage += "Severity: high\n\n"; break;
			case GL_DEBUG_SEVERITY_MEDIUM:       errorMessage += "Severity: medium\n\n"; break;
			case GL_DEBUG_SEVERITY_LOW:          errorMessage += "Severity: low\n\n"; break;
			case GL_DEBUG_SEVERITY_NOTIFICATION: errorMessage += "Severity: notification\n\n"; break;
		}

		MOE_ERROR(moe::ChanGraphics, errorMessage.c_str(), id, message);
	}
}

#endif // MOE_OPENGL