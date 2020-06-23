// Monocle Game Engine source files - Alexandre Baron


#ifdef MOE_OPENGL

#include "OpenGLGraphicsDevice.h"

#include "Graphics/VertexLayout/OpenGL/OpenGLVertexLayout.h"

#include "Graphics/VertexLayout/OpenGL/OpenGLVertexFormat.h"

#include "Graphics/Texture/OpenGL/OpenGLTextureFormat.h"

#include "Graphics/Pipeline/OpenGL/OpenGLPipeline.h"

#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>


namespace moe
{

	void OpenGLGraphicsDevice::Initialize()
	{
		m_vertexBufferPool.ReservePoolMemory(GL_DYNAMIC_STORAGE_BIT);
		m_indexBufferPool.ReservePoolMemory(GL_DYNAMIC_STORAGE_BIT);
		m_uniformBufferPool.ReservePoolMemory(GL_DYNAMIC_STORAGE_BIT);

		// Because OpenGL expects the 0.0 coordinate on the y-axis to be on the bottom-side of the image,
		// most images will appear vertically reversed in OpenGL since images usually have 0.0 at the top of the y-axis.
		// Luckily for us, stb_image.h can flip the y-axis during image loading :
		stbi_set_flip_vertically_on_load(true);
	}


	void OpenGLGraphicsDevice::Destroy()
	{
		m_shaderManager.Clear();

		// Destroy all VAOs at the same time.
		Vector<GLuint> vaoIDs(m_vertexLayouts.size());

		int iLayout = 0;
		for (const auto& layout : m_vertexLayouts)
		{
			vaoIDs[iLayout] = layout;
			iLayout++;
		}

		glDeleteVertexArrays((GLsizei)vaoIDs.Size(), vaoIDs.Data());

		m_vertexLayouts.clear();
	}


	void OpenGLGraphicsDevice::UpdateUniformBlockVariable(ShaderProgramHandle programHandle,
		DeviceBufferHandle targetBlockBuffer, const std::string& variableName, const void* data, size_t dataSizeBytes)
	{
		const OpenGLShaderProgram* program = m_shaderManager.GetProgram(programHandle);
		if (!MOE_ASSERT(program != nullptr))
		{
			// TODO: log error...
			return;
		}

		int variableOffset = program->GetBlockMemberOffset(variableName);

		UpdateUniformBuffer(targetBlockBuffer, data, dataSizeBytes, variableOffset);
	}


	GLuint OpenGLGraphicsDevice::UseShaderProgram(ShaderProgramHandle programHandle)
	{
		GLuint programID = GetShaderProgramID(programHandle);
		glUseProgram(programID);

		return programID;
	}


	GLuint OpenGLGraphicsDevice::GetShaderProgramID(ShaderProgramHandle programHandle)
	{
		const OpenGLShaderProgram* programPtr = m_shaderManager.GetProgram(programHandle);

		if (MOE_ASSERT(programPtr != nullptr))
		{
			GLuint shaderProgramID = (GLuint)(*programPtr);
			return shaderProgramID;
		}

		MOE_ERROR(ChanGraphics, "UseShaderProgram: requested an invalid shader program handle.");
		return 0;
	}


	VertexLayoutHandle OpenGLGraphicsDevice::CreateVertexLayout(const VertexLayoutDescriptor& vertexLayoutDesc)
	{
		// First and foremost, check that we do not have an existing vertex layout that could fit this description...
		auto vtxLayoutIt = std::find_if(m_vertexLayouts.begin(), m_vertexLayouts.end(), [&vertexLayoutDesc](const OpenGLVertexLayout& oglVtxLayout)
		{
			return (oglVtxLayout.ReadDescriptor() == vertexLayoutDesc);
		});


		if (vtxLayoutIt != m_vertexLayouts.end())
		{
			// we found a matching one : don't bother recreating a new one and give out a handle
			return VertexLayoutHandle{*vtxLayoutIt};
		}

		GLuint vaoID;
		glCreateVertexArrays(1, &vaoID);

		// Yeah it's a bit weird,don't initialize the handle just yet ! Will help to know if our algorithm worked.
		VertexLayoutHandle handle;

		uint32_t totalStride{ 0 }; // only really useful for Interleaved mode

		switch (vertexLayoutDesc.Type())
		{
			case LayoutType::Interleaved:
			{
				int iAttrib = 0;

				for (const VertexElementDescriptor& desc : vertexLayoutDesc)
				{
					auto OpenGLInfoOpt = OpenGLVertexElementFormat::TranslateFormat(desc.m_format);
					if (false == OpenGLInfoOpt.has_value())
					{
						MOE_ERROR(ChanGraphics, "Failed to translate following OpenGL format : %u", desc.m_format);
						break;
					}

					const auto& glVertexElemFmt = OpenGLInfoOpt.value();

					// TODO : switch to glVertexAttribPointer if retrocompat needed
					// Enable attrib index for given VAO
					glEnableVertexArrayAttrib(vaoID, iAttrib);

					// Specify format for this attrib : type; number of components, normalized. Use totalStride as relativeoffset because data is interleaved (XYZ XYZ...)
					glVertexArrayAttribFormat(vaoID, iAttrib, glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type, glVertexElemFmt.m_normalized, totalStride);

					// We can bind all attributes to same binding index because data is interleaved.
					glVertexArrayAttribBinding(vaoID, iAttrib, 0);

					auto sizeOpt = OpenGLVertexElementFormat::FindTypeSize(glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type);
					if (false == sizeOpt.has_value())
					{
						MOE_ERROR(ChanGraphics, "Failed to find the OpenGL type size of following combination : %u %i", glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type);
						break; // invalid, could not find component size
					}

					totalStride += sizeOpt.value();

					iAttrib++;
				}

				handle = VertexLayoutHandle{vaoID}; // if we're here initialization was successful until the end: set the handle.
			}
			break;

			case LayoutType::Packed:
			{
				int iAttrib = 0;

				for (const VertexElementDescriptor& desc : vertexLayoutDesc)
				{
					auto OpenGLInfoOpt = OpenGLVertexElementFormat::TranslateFormat(desc.m_format);
					if (false == OpenGLInfoOpt.has_value())
					{
						MOE_ERROR(ChanGraphics, "Failed to translate following OpenGL format : %u", desc.m_format);
						break;
					}

					const auto& glVertexElemFmt = OpenGLInfoOpt.value();

					// More of a safety measure because we actually don't need the total size in packed mode.
					auto sizeOpt = OpenGLVertexElementFormat::FindTypeSize(glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type);
					if (false == sizeOpt.has_value())
					{
						MOE_ERROR(ChanGraphics, "Failed to find the OpenGL type size of following combination : %u %i", glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type);
						break;
					}

					// TODO : switch to glVertexAttribPointer if retrocompat needed
					// Enable attrib index for given VAO
					glEnableVertexArrayAttrib(vaoID, iAttrib);

					// Specify format for this attrib : type; number of components, normalized. Use 0 as relativeoffset because data will be tightly packed (XXX YYY...)
					glVertexArrayAttribFormat(vaoID, iAttrib, glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type, glVertexElemFmt.m_normalized, 0);

					// As data will be tightly packed, we need to bind each attribute to a different binding index.
					glVertexArrayAttribBinding(vaoID, iAttrib, iAttrib);

					iAttrib++;
				}

				handle = VertexLayoutHandle{ vaoID }; // if we're here initialization was successful until the end: set the handle.
			}
			break;

			default:
			MOE_ASSERT(false); // not supposed to happen
		}

		if (handle.IsNull())
		{
			// There was a problem somewhere : delete our created VAO
			glDeleteVertexArrays(1, &vaoID);
		}
		else
		{
			// The VAO was successfully initialized : we store our vertex layout
			m_vertexLayouts.emplace(vertexLayoutDesc, vaoID, totalStride);
		}

		return handle;
	}


	VertexLayoutHandle OpenGLGraphicsDevice::CreateVertexLayout(InstancedVertexLayoutDescriptor desc)
	{
		GLuint vaoID;
		glCreateVertexArrays(1, &vaoID);

		// Yeah it's a bit weird,don't initialize the handle just yet ! Will help to know if our algorithm worked.
		VertexLayoutHandle handle;

		int iAttrib = 0;

		bool bindingInitSuccessful = true; // start optimistic

		for (auto iBindings = 0; bindingInitSuccessful && iBindings < desc.NumBindings(); iBindings++ )
		{
			auto& vtxBindings = *(desc.begin() + iBindings);
			uint32_t totalStride{ 0 }; // only really useful for Interleaved mode

			switch (vtxBindings.GetLayoutType())
			{
			case LayoutType::Interleaved:
				for (const VertexElementDescriptor& elemDesc : vtxBindings)
				{
					auto OpenGLInfoOpt = OpenGLVertexElementFormat::TranslateFormat(elemDesc.m_format);
					if (false == OpenGLInfoOpt.has_value())
					{
						MOE_ERROR(ChanGraphics, "Failed to translate following OpenGL format : %u", elemDesc.m_format);
						bindingInitSuccessful = false;
						break;
					}

					const auto& glVertexElemFmt = OpenGLInfoOpt.value();

					// "Repeats" are necessary to handle vertex attributes like mat4 that can exist in shaders, but are not handled by the API code.
					// For a mat4 as an example, we have to describe a matrix as four vec4 attributes following each other; the repeat count will then be 4.
					for (auto iRepeat = 0u; iRepeat < glVertexElemFmt.m_repeats; iRepeat++)
					{
						// TODO : switch to glVertexAttribPointer if retrocompat needed
						// Enable attrib index for given VAO
						glEnableVertexArrayAttrib(vaoID, iAttrib);

						// Specify format for this attrib : type; number of components, normalized. Use totalStride as relativeoffset because data is interleaved (XYZ XYZ...)
						glVertexArrayAttribFormat(vaoID, iAttrib, glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type, glVertexElemFmt.m_normalized, totalStride);

						// We can bind all attributes to same binding index because data is interleaved.
						glVertexArrayAttribBinding(vaoID, iAttrib, vtxBindings.BufferBinding());

						iAttrib++;

						auto sizeOpt = OpenGLVertexElementFormat::FindTypeSize(glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type);
						if (false == sizeOpt.has_value())
						{
							MOE_ERROR(ChanGraphics, "Failed to find the OpenGL type size of following combination : %u %i", glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type);
							bindingInitSuccessful = false;
							break; // invalid, could not find component size
						}

						totalStride += sizeOpt.value();
					}
				}

				if (vtxBindings.InstanceStepRate() != 0)
				{
					glVertexArrayBindingDivisor(vaoID, vtxBindings.BufferBinding(), vtxBindings.InstanceStepRate());
				}

				break;

			case LayoutType::Packed:
				for (const VertexElementDescriptor& elemDesc : vtxBindings)
				{
					auto OpenGLInfoOpt = OpenGLVertexElementFormat::TranslateFormat(elemDesc.m_format);
					if (false == OpenGLInfoOpt.has_value())
					{
						MOE_ERROR(ChanGraphics, "Failed to translate following OpenGL format : %u", elemDesc.m_format);
						break;
					}

					const auto& glVertexElemFmt = OpenGLInfoOpt.value();

					// More of a safety measure because we actually don't need the total size in packed mode.
					auto sizeOpt = OpenGLVertexElementFormat::FindTypeSize(glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type);
					if (false == sizeOpt.has_value())
					{
						MOE_ERROR(ChanGraphics, "Failed to find the OpenGL type size of following combination : %u %i", glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type);
						break;
					}

					// TODO : switch to glVertexAttribPointer if retrocompat needed
					// Enable attrib index for given VAO
					glEnableVertexArrayAttrib(vaoID, iAttrib);

					// Specify format for this attrib : type; number of components, normalized. Use 0 as relativeoffset because data will be tightly packed (XXX YYY...)
					glVertexArrayAttribFormat(vaoID, iAttrib, glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type, glVertexElemFmt.m_normalized, 0);

					// As data will be tightly packed, we need to bind each attribute to a different binding index.
					glVertexArrayAttribBinding(vaoID, iAttrib, vtxBindings.BufferBinding() + iAttrib);

					if (vtxBindings.InstanceStepRate() != 0)
					{
						glVertexArrayBindingDivisor(vaoID, vtxBindings.BufferBinding() + iAttrib, vtxBindings.InstanceStepRate());
					}

					iAttrib++;
				}
				break;

			default:
				MOE_ASSERT(false); // not supposed to happen
				MOE_ERROR(ChanGraphics, "Unmanaged vertex layout type.");
				break;
			}

			vtxBindings.SetTotalStride(totalStride);
		}

		if (bindingInitSuccessful)
		{
			handle = VertexLayoutHandle{ vaoID }; // if we're here initialization was successful until the end: set the handle.
			// The VAO was successfully initialized : we store our vertex layout
			m_vertexLayouts.emplace(std::move(desc), vaoID);
		}
		else
		{
			// There was a problem somewhere : delete our created VAO
			glDeleteVertexArrays(1, &vaoID);
		}

		return handle;
	}


	const VertexLayout* OpenGLGraphicsDevice::GetVertexLayout(VertexLayoutHandle handle) const
	{
		auto vtxLayout = m_vertexLayouts.find(handle);

		if (vtxLayout != m_vertexLayouts.end())
		{
			return &(*vtxLayout);
		}

		return nullptr;
	}


	const OpenGLVertexLayout* OpenGLGraphicsDevice::UseVertexLayout(VertexLayoutHandle layoutHandle)
	{
		const OpenGLVertexLayout* layout = static_cast<const OpenGLVertexLayout*>(GetVertexLayout(layoutHandle));

		if (MOE_ASSERT(layout != nullptr))
		{
			glBindVertexArray((GLuint)*layout);
		}
		else
		{
			MOE_ERROR(ChanGraphics, "UseVertexLayout was passed an invalid layout handle.");
		}

		return layout;
	}


	DeviceBufferHandle OpenGLGraphicsDevice::CreateStaticVertexBuffer(const void* data, size_t byteSize)
	{
		uint32_t meshOffset = m_vertexBufferPool.Allocate(data, (uint32_t)byteSize);

		GLuint bufferID;

		if (meshOffset == OpenGLBuddyAllocator::ms_INVALID_OFFSET)
		{
			// The requested allocation didn't fit in our allocation scheme : allocate an ad hoc buffer.
			// TODO: this is very bad ! This allocation goes completely off the grid ! It should be recorded somewhere...
			glCreateBuffers(1, &bufferID);

			MOE_DEBUG_ASSERT(bufferID != 0);

			glNamedBufferStorage(bufferID, byteSize, data, GL_DYNAMIC_STORAGE_BIT);

			meshOffset = 0;
		}
		else
		{
			bufferID = m_vertexBufferPool.GetBufferHandle();
		}

		// Encode the VBO ID and the offset in the handle.
		// The handle looks like : | VBO ID (32 bits) | offset in VBO (32 bits) |
		DeviceBufferHandle bufHandle = EncodeBufferHandle(bufferID, meshOffset);
		return bufHandle;
	}


	void OpenGLGraphicsDevice::DeleteStaticVertexBuffer(DeviceBufferHandle vtxHandle)
	{
		if (!MOE_ASSERT(vtxHandle.IsNotNull()))
		{
			return ; // not supposed to happen
		}

		// Right now, the pool only needs the offset (we use a single VBO).

		// Controlled narrowing conversion to only keep the 32 least-significant bits (containing the offset value)
		uint32_t offset = (uint32_t)vtxHandle.Get();

		m_vertexBufferPool.Free(offset);
	}


	DeviceBufferHandle OpenGLGraphicsDevice::CreateIndexBuffer(const void* indexData, size_t indexDataSizeBytes)
	{
		const uint32_t indexOffset = m_indexBufferPool.Allocate(indexData, (uint32_t)indexDataSizeBytes);
		if (indexOffset == OpenGLBuddyAllocator::ms_INVALID_OFFSET)
		{
			return DeviceBufferHandle::Null();
		}
		else
		{
			// Encode the EBO ID and the offset in the handle.
			// The handle looks like : | EBO ID (32 bits) | offset in EBO (32 bits) |
			uint64_t handleValue = (uint64_t)m_indexBufferPool.GetBufferHandle() << 32;
			handleValue |= indexOffset;
			return DeviceBufferHandle{ handleValue };
		}
	}


	void OpenGLGraphicsDevice::DeleteIndexBuffer(DeviceBufferHandle idxHandle)
	{
		if (!MOE_ASSERT(idxHandle.IsNotNull()))
		{
			return; // not supposed to happen
		}

		// Right now, the pool only needs the offset (we use a single EBO).

		// Controlled narrowing conversion to only keep the 32 least-significant bits (containing the offset value)
		uint32_t offset = (uint32_t)idxHandle.Get();

		m_indexBufferPool.Free(offset);
	}


	void OpenGLGraphicsDevice::DrawVertexBuffer(VertexLayoutHandle vtxLayoutHandle, DeviceBufferHandle vtxBufHandle, size_t numVertices,
		DeviceBufferHandle idxBufHandle, size_t numIndices)
	{
		const OpenGLVertexLayout* vtxLayout = UseVertexLayout(vtxLayoutHandle);
		if (vtxLayout == nullptr)
		{
			return;
		}


		auto[vbo, vboOffset] = DecodeBufferHandle(vtxBufHandle);

		if (vtxLayout->IsInterleaved())
		{
			glVertexArrayVertexBuffer(vtxLayout->VAO(), 0, vbo, vboOffset, vtxLayout->GetStrideBytes());
		}
		else
		{
			// In packed mode, set the bindings one by one.
			const VertexLayoutDescriptor& layoutDesc = vtxLayout->ReadDescriptor();

			uint32_t bindingIdx = 0;
			size_t elemBufferOffset = vboOffset;

			for (const VertexElementDescriptor& elemDesc : layoutDesc)
			{
				auto oglElemFormat = OpenGLVertexElementFormat::TranslateFormat(elemDesc.m_format);
				auto typeSize = OpenGLVertexElementFormat::FindTypeSize(oglElemFormat.value().m_numCpnts, oglElemFormat.value().m_type);
				glVertexArrayVertexBuffer(vtxLayout->VAO(), bindingIdx, vbo, elemBufferOffset, typeSize.value());

				elemBufferOffset += numVertices * typeSize.value();
				bindingIdx++;
			}
		}

		if (idxBufHandle.IsNotNull())
		{
			auto[ebo, eboOffset] = DecodeBufferHandle(idxBufHandle);

			glVertexArrayElementBuffer(vtxLayout->VAO(), ebo);

			glDrawElements(m_primitiveTopology, (GLsizei)numIndices, GL_UNSIGNED_INT, (const void*)((uint64_t)eboOffset));
		}
		else
		{
			glDrawArrays(m_primitiveTopology, 0, (GLsizei)numVertices);
		}
	}


	void OpenGLGraphicsDevice::DrawInstancedMesh(VertexLayoutHandle vtxLayoutHandle, DeviceBufferHandle vtxBufHandle,
		size_t numVertices, DeviceBufferHandle idxBufHandle, size_t numIndices, DeviceBufferHandle instancingBuffer, uint32_t instancesAmount)
	{
		const OpenGLVertexLayout* vtxLayout = UseVertexLayout(vtxLayoutHandle);
		if (vtxLayout == nullptr)
		{
			return;
		}

		for (const auto& vtxBindings : vtxLayout->ReadInstancedDescriptor())
		{
			std::pair<GLuint, unsigned> bufferIDAndOffset;

			if (vtxBindings.InstanceStepRate() == 0)
			{
				bufferIDAndOffset = DecodeBufferHandle(vtxBufHandle );
			}
			else
			{
				bufferIDAndOffset = DecodeBufferHandle(instancingBuffer);
			}

			if (vtxBindings.IsInterleaved())
			{
				glVertexArrayVertexBuffer(vtxLayout->VAO(), vtxBindings.BufferBinding(), bufferIDAndOffset.first, bufferIDAndOffset.second, vtxBindings.GetTotalStride());
			}
			else
			{
				// In packed mode, set the bindings one by one.
				const VertexLayoutDescriptor& layoutDesc = vtxLayout->ReadDescriptor();

				uint32_t bindingIdx = 0;
				size_t elemBufferOffset = bufferIDAndOffset.second;

				for (const VertexElementDescriptor& elemDesc : layoutDesc)
				{
					auto oglElemFormat = OpenGLVertexElementFormat::TranslateFormat(elemDesc.m_format);
					auto typeSize = OpenGLVertexElementFormat::FindTypeSize(oglElemFormat.value().m_numCpnts, oglElemFormat.value().m_type);
					glVertexArrayVertexBuffer(vtxLayout->VAO(), bindingIdx, bufferIDAndOffset.first, elemBufferOffset, typeSize.value());

					elemBufferOffset += numVertices * typeSize.value();
					bindingIdx++;
				}
			}
		}

		if (idxBufHandle.IsNotNull())
		{
			auto[ebo, eboOffset] = DecodeBufferHandle(idxBufHandle);

			glVertexArrayElementBuffer(vtxLayout->VAO(), ebo);

			glDrawElementsInstanced(m_primitiveTopology, (GLsizei)numIndices, GL_UNSIGNED_INT, (const void*)(uint64_t)eboOffset, instancesAmount);
		}
		else
		{
			glDrawArraysInstanced(m_primitiveTopology, 0, (GLsizei)numVertices, instancesAmount);
		}
	}


	void OpenGLGraphicsDevice::UpdateBuffer(DeviceBufferHandle bufferHandle, const void* data, size_t dataSize) const
	{
		auto [ubo, uboOffset] = DecodeBufferHandle(bufferHandle);

		glNamedBufferSubData(ubo, uboOffset, dataSize, data);
	}



	ViewportHandle OpenGLGraphicsDevice::CreateViewport(const ViewportDescriptor& vpDesc)
	{
		FreelistID newVpId = m_viewports.Add(vpDesc);
		return newVpId.ToHandle<ViewportHandle>();
	}


	void OpenGLGraphicsDevice::UseViewport(ViewportHandle vpHandle)
	{
		const ViewportDescriptor& desc = m_viewports.Lookup(vpHandle.Get() - 1);

		glViewport((GLint)desc.m_x, (GLint)desc.m_y, (GLsizei)desc.m_width, (GLsizei)desc.m_height);
	}


	DeviceBufferHandle OpenGLGraphicsDevice::CreateUniformBuffer(const void* uniformData, size_t uniformDataSizeBytes)
	{
		const uint32_t uboOffset = m_uniformBufferPool.Allocate(uniformData, (uint32_t)uniformDataSizeBytes);
		if (uboOffset == OpenGLBuddyAllocator::ms_INVALID_OFFSET)
		{
			return DeviceBufferHandle::Null();
		}
		else
		{
			// Encode the UBO ID and the offset in the handle.
			// The handle looks like : | UBO ID (32 bits) | offset in UBO (32 bits) |
			uint64_t handleValue = (uint64_t)m_uniformBufferPool.GetBufferHandle() << 32;
			handleValue |= uboOffset;

			DeviceBufferHandle newBufferHandle{handleValue};

			// Don't forget to store the size, it will be useful when using the buffer.
			m_uniformBufferSizes[newBufferHandle] = (uint32_t)uniformDataSizeBytes;

			return newBufferHandle;
		}
	}


	ResourceLayoutHandle OpenGLGraphicsDevice::CreateResourceLayout(const ResourceLayoutDescriptor& newDesc)
	{
		FreelistID newLayoutID = m_resourceLayouts.Add(newDesc);
		return newLayoutID.ToHandle<ResourceLayoutHandle>();
	}


	ResourceSetHandle OpenGLGraphicsDevice::CreateResourceSet(const ResourceSetDescriptor& newDesc)
	{
		FreelistID newSetID = m_resourceSets.Add(newDesc);
		return newSetID.ToHandle<ResourceSetHandle>();
	}


	Texture2DHandle OpenGLGraphicsDevice::CreateTexture2D(const Texture2DDescriptor& tex2DDesc)
	{
		// First ensure the target texture format is valid - don't bother going further if not
		const GLuint GLtextureFormat = TranslateToOpenGLSizedFormat(tex2DDesc.m_targetFormat);
		if (GLtextureFormat == 0)
		{
			return Texture2DHandle::Null();
		}

		// we have the data: upload to GPU
		GLuint textureID;

		// If the required texture is a write-only (not Sampled) render target, use a renderbuffer object instead.
		// Renderbuffer objects store all the render data directly into their buffer without any conversions to texture-specific formats.
		// It makes them faster as a writeable storage medium, as they are write-only objects.
		if (tex2DDesc.m_texUsage & RenderTarget && (tex2DDesc.m_texUsage & Sampled) == 0)
		{
			unsigned int rbo;
			glCreateRenderbuffers(1, &rbo);
			glNamedRenderbufferStorage(rbo, GLtextureFormat, tex2DDesc.m_width, tex2DDesc.m_height);

			return Texture2DHandle{ EncodeRenderbufferHandle(rbo).Get() };
		}
		else
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
			glTextureStorage2D(textureID, tex2DDesc.m_wantedMipmapLevels, GLtextureFormat, tex2DDesc.m_width, tex2DDesc.m_height);
			if (tex2DDesc.m_imageData != nullptr)
			{
				glTextureSubImage2D(textureID, 0, 0, 0, tex2DDesc.m_width, tex2DDesc.m_height, GL_RGBA, GL_UNSIGNED_BYTE, tex2DDesc.m_imageData);
			}

			if (tex2DDesc.m_wantedMipmapLevels != 0)
			{
				glGenerateTextureMipmap(textureID);
			}

			return Texture2DHandle{ textureID };
		}
	}


	Texture2DHandle OpenGLGraphicsDevice::CreateTexture2D(const Texture2DFileDescriptor& tex2DFileDesc)
	{
		// First ensure the target texture format is valid - don't bother going further if not
		const GLuint textureFormat = TranslateToOpenGLSizedFormat(tex2DFileDesc.m_targetFormat);
		if (textureFormat == 0)
		{
			return Texture2DHandle::Null();
		}

		// Now read the file, taking into account required number of components.
		const std::uint32_t requiredCompNum = GetTextureFormatChannelsNumber(tex2DFileDesc.m_requiredFormat);

		int width, height, nrChannels;
		unsigned char * const imageData = stbi_load(tex2DFileDesc.m_filename.c_str(), &width, &height, &nrChannels, requiredCompNum);

		if (imageData == nullptr)
		{
			MOE_ERROR(ChanGraphics, "Image file %s could not be read.", tex2DFileDesc.m_filename);
			MOE_DEBUG_ASSERT(false);
			return Texture2DHandle::Null();
		}

		// we have the data: upload to GPU

		// Determine what could be a good base format
		const GLuint inputBaseFormat = TranslateToOpenGLBaseFormat(nrChannels);

		GLuint textureID;

		glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
		glTextureStorage2D(textureID, tex2DFileDesc.m_wantedMipmapLevels, textureFormat, width, height);
		glTextureSubImage2D(textureID, 0, 0, 0, width, height, inputBaseFormat, GL_UNSIGNED_BYTE, imageData);


		if (tex2DFileDesc.m_wantedMipmapLevels != 0)
		{
			glGenerateTextureMipmap(textureID);
		}

		// we don't need to keep the image data
		stbi_image_free(imageData);

		return Texture2DHandle{ textureID };
	}


	TextureHandle OpenGLGraphicsDevice::CreateCubemapTexture(const CubeMapTextureFilesDescriptor& cubemapFilesDesc)
	{
		// First ensure the target texture format is valid - don't bother going further if not
		const GLuint textureFormat = TranslateToOpenGLSizedFormat(cubemapFilesDesc.m_targetFormat);
		if (textureFormat == 0)
		{
			MOE_DEBUG_ASSERT(false); // not supposed to happen
			return TextureHandle::Null();
		}

		// Now read the files, taking into account required number of components.
		const std::uint32_t requiredCompNum = GetTextureFormatChannelsNumber(cubemapFilesDesc.m_requiredFormat);

		unsigned char* imagesData[6]{nullptr};
		int iCubeSide = 0;
		int cubemapWidth, cubemapHeight, cubemapNrChannels;
		cubemapWidth = cubemapHeight = cubemapNrChannels = 0;

		bool failed = false; // start optimistic

		// contrary to the usual OpenGL behaviour of having the image origin in the lower left,
		// Cube Maps' specification is that the images' origin is in the upper left.
		// So specifically when loading cube maps, we have to revert our vertical flipping image loading logic.
		// cf. https://stackoverflow.com/questions/11685608/convention-of-faces-in-opengl-cubemapping
		stbi_set_flip_vertically_on_load(false);

		for (const auto& fileName : cubemapFilesDesc.m_rightLeftTopBottomFrontBackTexFiles)
		{
			int width, height, nrChannels;
			imagesData[iCubeSide] = stbi_load(fileName.c_str(), &width, &height, &nrChannels, requiredCompNum);

			if (imagesData[iCubeSide] == nullptr)
			{
				MOE_ERROR(ChanGraphics, "Could not create cube map : image file %s could not be read.", fileName);
				failed = true;
				break;
			}

			if (cubemapWidth == 0)
			{
				// First round : initialize the cube maps values
				cubemapWidth = width;
				cubemapHeight = height;
				cubemapNrChannels = nrChannels;
			}
			else
			{
				// Sanity check : make sure all cubemap images have the same width, height and channels number information (or something may be wrong)
				if (width != cubemapWidth || height != cubemapHeight || nrChannels != cubemapNrChannels)
				{
					MOE_ERROR(ChanGraphics, "Could not create cube map : image file %s data format differs from other cube map files !", fileName);
					failed = true;
					break;
				}
			}

			iCubeSide++;
		}

		// Turn the vertical flip back on
		stbi_set_flip_vertically_on_load(true);

		if (!MOE_ASSERT(!failed))
		{
			// Something went wrong: free the data and stop here
			for (unsigned char* imgData : imagesData)
			{
				if (imgData != nullptr)
					stbi_image_free(imgData);
			}
			return TextureHandle::Null();
		}

		// From now on we successfully loaded image data : upload to GPU

		// Determine what could be a good base format
		const GLuint inputBaseFormat = TranslateToOpenGLBaseFormat(cubemapNrChannels);

		// Creating a cube map with DSA requires us to use glTextureStorage2D + glTextureSubImage3D.
		// For more info, see : https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions#uploading-cube-maps
		// Or : https://www.reddit.com/r/opengl/comments/556zac/how_to_create_cubemap_with_direct_state_access/
		GLuint cubemapID;
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &cubemapID);
		glTextureStorage2D(cubemapID, cubemapFilesDesc.m_wantedMipmapLevels, textureFormat, cubemapWidth, cubemapHeight);
		for (GLint face = 0; face < 6; ++face)
		{
			glTextureSubImage3D(cubemapID, 0, 0, 0, face, cubemapWidth, cubemapHeight, 1, inputBaseFormat, GL_UNSIGNED_BYTE, imagesData[face]);
		}

		if (cubemapFilesDesc.m_wantedMipmapLevels != 0)
		{
			glGenerateTextureMipmap(cubemapID);
		}

		// We set the wrapping method to GL_CLAMP_TO_EDGE since texture coordinates that are exactly between two faces may not hit an exact face (due to some hardware limitations).
		// So by using GL_CLAMP_TO_EDGE OpenGL always returns their edge values whenever we sample between faces.
		// TODO: use a sampler to do that ?
		glTextureParameteri(cubemapID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(cubemapID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(cubemapID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(cubemapID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(cubemapID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// we don't need to keep the image data
		for (unsigned char* imgData : imagesData)
		{
			if (imgData != nullptr)
				stbi_image_free(imgData);
		}

		return TextureHandle{ cubemapID };
	}


	TextureHandle OpenGLGraphicsDevice::CreateCubemapTexture(const CubeMapTextureDescriptor& cubemapDesc)
	{
		// First ensure source and target texture formats are valid - don't bother going further if not
		const GLuint sourceFormat = TranslateToOpenGLSizedFormat(cubemapDesc.m_sourceFormat);
		const GLuint storageFormat = TranslateToOpenGLSizedFormat(cubemapDesc.m_targetFormat);
		if (sourceFormat == 0 || storageFormat == 0)
		{
			MOE_DEBUG_ASSERT(false); // not supposed to happen
			return Texture2DHandle::Null();
		}

		// Creating a cube map with DSA requires us to use glTextureStorage2D + glTextureSubImage3D.
		// For more info, see : https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions#uploading-cube-maps
		// Or : https://www.reddit.com/r/opengl/comments/556zac/how_to_create_cubemap_with_direct_state_access/
		GLuint cubemapID;
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &cubemapID);
		glTextureStorage2D(cubemapID, cubemapDesc.m_wantedMipmapLevels, storageFormat, cubemapDesc.m_width, cubemapDesc.m_height);

		uint8_t nbrComponents = 0;
		unsigned imageByteOffset = 0;

		if (cubemapDesc.m_imageData != nullptr) // Don't bother making these calculations if there is no data anyway...
		{
			nbrComponents = GetTextureFormatChannelsNumber(cubemapDesc.m_sourceFormat);
			imageByteOffset = cubemapDesc.m_width * cubemapDesc.m_height * nbrComponents;
		}

		// The data upload assumes the 6 images are stored contiguously in memory at address imageData.
		// Note : we cannot pass a nullptr to glTextureSubImage3D.
		if (cubemapDesc.m_imageData != nullptr)
		{
			for (GLint face = 0; face < 6; ++face)
			{
				const void* imagePtr = (cubemapDesc.m_imageData != nullptr ? cubemapDesc.m_imageData + (face * imageByteOffset) : nullptr);
				glTextureSubImage3D(cubemapID, 0, 0, 0, face, cubemapDesc.m_width, cubemapDesc.m_height, 1, sourceFormat, GL_UNSIGNED_BYTE, imagePtr);
			}

			if (cubemapDesc.m_wantedMipmapLevels != 0)
			{
				glGenerateTextureMipmap(cubemapID);
			}
		}

		// We set the wrapping method to GL_CLAMP_TO_EDGE since texture coordinates that are exactly between two faces may not hit an exact face (due to some hardware limitations).
		// So by using GL_CLAMP_TO_EDGE OpenGL always returns their edge values whenever we sample between faces.
		// TODO: this is application dependent, use a sampler to do that !
		glTextureParameteri(cubemapID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(cubemapID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(cubemapID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(cubemapID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(cubemapID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return TextureHandle{ cubemapID };
	}


	void OpenGLGraphicsDevice::DestroyTexture2D(Texture2DHandle texHandle)
	{
		GLuint texID{texHandle.Get()};
		glDeleteTextures(1, &texID);
	}


	uint32_t OpenGLGraphicsDevice::GetShaderProgramUniformBlockSize(ShaderProgramHandle shaderHandle, const std::string& /*uniformBlockName*/)
	{
		const OpenGLShaderProgram* programPtr = m_shaderManager.GetProgram(shaderHandle);

		if (!MOE_ASSERT(programPtr != nullptr))
		{
			return UINT32_MAX;
		}

		GLuint shaderProgramID = (GLuint)(*programPtr);

		GLint numBlocks = 0;
		glGetProgramInterfaceiv(shaderProgramID, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks);
		const GLenum blockProperties[1] = { GL_NUM_ACTIVE_VARIABLES };
		const GLenum activeUnifProp[1] = { GL_ACTIVE_VARIABLES };
		const GLenum unifProperties[3] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };

		for (int blockIx = 0; blockIx < numBlocks; ++blockIx)
		{

			GLint numActiveUnifs = 0;
			glGetProgramResourceiv(shaderProgramID, GL_UNIFORM_BLOCK, blockIx, 1, blockProperties, 1, NULL, &numActiveUnifs);

			if (!numActiveUnifs)
				continue;

			std::vector<GLint> blockUnifs(numActiveUnifs);
			glGetProgramResourceiv(shaderProgramID, GL_UNIFORM_BLOCK, blockIx, 1, activeUnifProp, numActiveUnifs, NULL, &blockUnifs[0]);

			for (int unifIx = 0; unifIx < numActiveUnifs; ++unifIx)
			{
				GLint values[3];
				glGetProgramResourceiv(shaderProgramID, GL_UNIFORM, blockUnifs[unifIx], 3, unifProperties, 3, NULL, values);

				// Get the name. Must use a std::vector rather than a std::string for C++03 standards issues.
				// C++11 would let you use a std::string directly.
				std::vector<char> nameData(values[0]);
				glGetProgramResourceName(shaderProgramID, GL_UNIFORM, blockUnifs[unifIx], (GLsizei)nameData.size(), NULL, &nameData[0]);
				std::string name(nameData.begin(), nameData.end() - 1);
				const char* nameStr = name.c_str();
				unsigned int indices[1];
				GLint offsets[1];

				glGetUniformIndices(shaderProgramID, 1, &nameStr, indices);
				glGetActiveUniformsiv(shaderProgramID, 1, indices, GL_UNIFORM_OFFSET, offsets);

			}
		}

		return 0;
	}


	bool OpenGLGraphicsDevice::IsPartOfUniformBlock(ShaderProgramHandle shaderHandle, const std::string& uniformBlockName, const std::string& uniformMemberName) const
	{
		const OpenGLShaderProgram* programPtr = m_shaderManager.GetProgram(shaderHandle);

		if (!MOE_ASSERT(programPtr != nullptr))
		{
			return false;
		}

		GLuint shaderProgramID = (GLuint)(*programPtr);

		GLint numBlocks;
		glGetProgramiv(shaderProgramID, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);


		std::vector<std::string> nameList;
		nameList.reserve(numBlocks);

		for (int blockIx = 0; blockIx < numBlocks; ++blockIx)
		{
			GLint nameLen;
			glGetActiveUniformBlockiv(shaderProgramID, blockIx, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);

			std::string blockName;
			blockName.resize(nameLen);
			glGetActiveUniformBlockName(shaderProgramID, blockIx, nameLen, nullptr, &blockName[0]);

			if (blockName != uniformBlockName)
				continue;

			const GLenum blockProperties[1] = { GL_NUM_ACTIVE_VARIABLES };
			const GLenum activeUnifProp[1] = { GL_ACTIVE_VARIABLES };
			const GLenum unifProperties[1] = { GL_NAME_LENGTH };

			for (int blockVarIdx = 0; blockVarIdx < numBlocks; ++blockVarIdx)
			{
				GLint numActiveUnifs = 0;
				glGetProgramResourceiv(shaderProgramID, GL_UNIFORM_BLOCK, blockVarIdx, 1, blockProperties, 1, NULL, &numActiveUnifs);

				if (!numActiveUnifs)
					continue;

				std::vector<GLint> blockUnifs(numActiveUnifs);
				glGetProgramResourceiv(shaderProgramID, GL_UNIFORM_BLOCK, blockVarIdx, 1, activeUnifProp, numActiveUnifs, NULL, &blockUnifs[0]);

				for (int unifIx = 0; unifIx < numActiveUnifs; ++unifIx)
				{
					GLint values[1];
					glGetProgramResourceiv(shaderProgramID, GL_UNIFORM, blockUnifs[unifIx], 1, unifProperties, 1, NULL, values);

					// Get the name. Must use a std::vector rather than a std::string for C++03 standards issues.
					// C++11 would let you use a std::string directly.
					std::vector<char> nameData(values[0]);
					glGetProgramResourceName(shaderProgramID, GL_UNIFORM, blockUnifs[unifIx], (GLsizei)nameData.size(), NULL, &nameData[0]);
					std::string name(nameData.begin(), nameData.end() - 1);

					if (name == uniformMemberName)
						return true;
				}
			}
		}

		return false;
	}


	void OpenGLGraphicsDevice::BindProgramUniformBlock(GLuint shaderProgramID, const char* uniformBlockName, int uniformBlockBinding, DeviceBufferHandle ubHandle)
	{
		// First retrieve the size of our uniform buffer or early exit...
		auto sizeIt = m_uniformBufferSizes.Find(ubHandle);
		if (!MOE_ASSERT(sizeIt != m_uniformBufferSizes.End()))
		{
			return;
		}

		auto [ubo, uboOffset] = DecodeBufferHandle(ubHandle);

		const unsigned int uniformBlockProgramIndex = glGetUniformBlockIndex(shaderProgramID, uniformBlockName);

		if (uniformBlockProgramIndex != UINT32_MAX)
		{
			glUniformBlockBinding(shaderProgramID, uniformBlockProgramIndex, uniformBlockBinding);
			glBindBufferRange(GL_UNIFORM_BUFFER, uniformBlockBinding, ubo, uboOffset, sizeIt->second);
		}
		else
		{
			MOE_ASSERT(false);
			MOE_ERROR(ChanGraphics, "Tried to use block %s in shader program %u but this block was not found inside the shader.", uniformBlockName, shaderProgramID);
		}
	}


	void OpenGLGraphicsDevice::BindUniformBlock(unsigned int uniformBlockBinding, DeviceBufferHandle ubHandle, uint32_t bufferSize, uint32_t relativeOffset)
	{
		// First retrieve the size of our uniform buffer or early exit...
		if (bufferSize == 0)
		{
			auto sizeIt = m_uniformBufferSizes.Find(ubHandle);
			if (!MOE_ASSERT(sizeIt != m_uniformBufferSizes.End()))
			{
				return;
			}

			bufferSize = sizeIt->second;
		}

		auto[ubo, uboOffset] = DecodeBufferHandle(ubHandle);

		glBindBufferRange(GL_UNIFORM_BUFFER, uniformBlockBinding, ubo, uboOffset + relativeOffset, bufferSize);
	}


	void OpenGLGraphicsDevice::UpdateUniformBuffer(DeviceBufferHandle ubHandle, const void* data, size_t dataSizeBytes, uint32_t relativeOffset)
	{
		auto[ubo, uboOffset] = DecodeBufferHandle(ubHandle);

		glNamedBufferSubData(ubo, uboOffset + relativeOffset, dataSizeBytes, data);
	}


	void OpenGLGraphicsDevice::BindTextureUnitToProgramUniform(GLuint shaderProgramID, int textureUnitIndex, TextureHandle texHandle, const char* uniformName)
	{
		glBindTextureUnit(textureUnitIndex, texHandle.Get());
		glProgramUniform1i(shaderProgramID, glGetUniformLocation(shaderProgramID, uniformName), textureUnitIndex);
	}


	void OpenGLGraphicsDevice::BindTextureUnit(int textureBindingPoint, TextureHandle texHandle)
	{
		glBindTextureUnit(textureBindingPoint, texHandle.Get());

	}


	PipelineHandle OpenGLGraphicsDevice::CreatePipeline(PipelineDescriptor& pipelineDesc)
	{
		// TODO: make a move version (or take a PipelineDescriptor without ref and move it)
		// TODO: we should store a "OpenGLPipelineDescriptor" to avoid having to translate pipeline values on every SetPipeline.
		FreelistID pipelineID = m_pipelines.Add(pipelineDesc);
		return pipelineID.ToHandle<PipelineHandle>();
	}


	void OpenGLGraphicsDevice::SetPipeline(PipelineHandle pipeHandle)
	{
		const PipelineDescriptor& pipelineDesc = m_pipelines.Lookup(pipeHandle.Get() - 1);

		// Program the OpenGL pipeline to fit the description of the given pipeline.
		OpenGLPipeline::SetBlendState(pipelineDesc.m_blendStateDesc);

		OpenGLPipeline::SetDepthStencilState(pipelineDesc.m_depthStencilStateDesc);

		OpenGLPipeline::SetRasterizerState(pipelineDesc.m_rasterizerStateDesc);

		m_primitiveTopology = OpenGLPipeline::GetOpenGLPrimitiveTopology(pipelineDesc.m_topology);
	}


	SwapchainHandle OpenGLGraphicsDevice::CreateSwapChain(uint32_t renderWidth, uint32_t renderHeight, FramebufferAttachment wantedAttachments)
	{
		m_swapChain.Create(*this, renderWidth, renderHeight, wantedAttachments);

		return SwapchainHandle{1};
	}


	FramebufferHandle OpenGLGraphicsDevice::CreateFramebuffer(const FramebufferDescriptor& fbDesc)
	{
		m_framebuffers.EmplaceBack(fbDesc);
		return FramebufferHandle{ (FramebufferHandle::Underlying) m_framebuffers.Back().GetID() };
	}


	SamplerHandle OpenGLGraphicsDevice::CreateSampler(const SamplerDescriptor& samplerDesc)
	{
		GLuint newSamplerID;
		glCreateSamplers(1, &newSamplerID);

		// Wrapping parameters
		auto wrapS = OpenGLSampler::TranslateToOpenGLWrapMode(samplerDesc.m_wrap_S);
		auto wrapT = OpenGLSampler::TranslateToOpenGLWrapMode(samplerDesc.m_wrap_T);
		auto wrapR = OpenGLSampler::TranslateToOpenGLWrapMode(samplerDesc.m_wrap_R);
		glSamplerParameteri(newSamplerID, GL_TEXTURE_WRAP_S, wrapS);
		glSamplerParameteri(newSamplerID, GL_TEXTURE_WRAP_T, wrapT);
		glSamplerParameteri(newSamplerID, GL_TEXTURE_WRAP_R, wrapR);

		glSamplerParameterfv(newSamplerID, GL_TEXTURE_BORDER_COLOR, samplerDesc.m_borderColor.Ptr());

		// Filtering parameters
		auto magFilter = OpenGLSampler::TranslateToOpenGLFilterMode(samplerDesc.m_magFilter);
		auto minFilter = OpenGLSampler::TranslateToOpenGLFilterMode(samplerDesc.m_minFilter);
		glSamplerParameteri(newSamplerID, GL_TEXTURE_MAG_FILTER, magFilter);
		glSamplerParameteri(newSamplerID, GL_TEXTURE_MIN_FILTER, minFilter);

		// Anisotropic filtering
		float usedAnisotropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &usedAnisotropy);
		usedAnisotropy = std::min(usedAnisotropy, samplerDesc.m_anisotropy);
		glSamplerParameterf(newSamplerID, GL_TEXTURE_MAX_ANISOTROPY_EXT, usedAnisotropy); // TODO : check this extension is here

		// LOD range
		glSamplerParameterf(newSamplerID, GL_TEXTURE_MIN_LOD, samplerDesc.m_lodRangeMin);
		glSamplerParameterf(newSamplerID, GL_TEXTURE_MAX_LOD, samplerDesc.m_lodRangeMax);

		// LOD bias
		float usedLodBias;
		glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &usedLodBias);
		// Clamp to [-maxBias, maxBias] range.
		usedLodBias = std::min(usedLodBias, std::max(-usedLodBias, samplerDesc.m_lodBias));
		glSamplerParameterf(newSamplerID, GL_TEXTURE_LOD_BIAS, usedLodBias);

		auto freeListID = m_samplers.Add(newSamplerID, samplerDesc);
		MOE_DEBUG_ASSERT(freeListID.Index() < (1 << 16)); // We cannot have more than 65k samplers at that time.

		SamplerHandle newHandle = EncodeSamplerHandle(newSamplerID, freeListID.Index());

		return newHandle;
	}


	void OpenGLGraphicsDevice::BindSamplerToTextureUnit(int textureBindingPoint, SamplerHandle samplerHandle)
	{
		auto [samplerID, _ ] = DecodeSamplerHandle(samplerHandle);
		glBindSampler(textureBindingPoint, samplerID);
	}


	std::pair<uint16_t, uint16_t> OpenGLGraphicsDevice::DecodeSamplerHandle(SamplerHandle handleToDecode)
	{
		uint16_t samplerID = handleToDecode.Get() >> 16;
		uint16_t freeListID = (uint16_t)handleToDecode.Get();
		return { samplerID, freeListID};
	}


	SamplerHandle OpenGLGraphicsDevice::EncodeSamplerHandle(unsigned samplerID, unsigned freelistID)
	{
		uint32_t handleValue = (uint16_t)samplerID << 16 | (uint16_t)freelistID;
		return SamplerHandle{ handleValue };
	}


	DeviceBufferHandle OpenGLGraphicsDevice::EncodeBufferHandle(uint32_t bufferID, uint32_t bufferOffset)
	{
		uint64_t handleValue = (uint64_t)bufferID << 32;
		handleValue |= bufferOffset;
		return DeviceBufferHandle{ handleValue };
	}


	std::pair<unsigned, unsigned> OpenGLGraphicsDevice::DecodeBufferHandle(
		const RenderObjectHandle<std::uint64_t>& handle)
	{
		uint64_t handleVal = handle.Get();
		uint32_t bufferID = handleVal >> 32;
		uint32_t bufferOffset = (uint32_t)handleVal;
		return {bufferID, bufferOffset};
	}


}

#endif