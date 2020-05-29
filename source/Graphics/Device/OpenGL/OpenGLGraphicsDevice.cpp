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
		Vector<GLuint> vaoIDs(m_vertexLayouts.Size());

		int iLayout = 0;
		for (const auto& layout : m_vertexLayouts)
		{
			vaoIDs[iLayout] = layout;
			iLayout++;
		}

		glDeleteVertexArrays((GLsizei)vaoIDs.Size(), vaoIDs.Data());

		m_vertexLayouts.Clear();
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
		// It's ok to linear search we don't expect a lot of existing vertex layouts anyway
		auto vtxLayoutIt = std::find_if(m_vertexLayouts.Begin(), m_vertexLayouts.End(), [&vertexLayoutDesc](const OpenGLVertexLayout& oglVtxLayout)
		{
			return (oglVtxLayout.ReadDescriptor() == vertexLayoutDesc);
		});

		if (vtxLayoutIt != m_vertexLayouts.End())
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
			case VertexLayoutDescriptor::Interleaved:
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

			case VertexLayoutDescriptor::Packed:
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
			m_vertexLayouts.EmplaceBack(vertexLayoutDesc, vaoID, totalStride);
		}

		return handle;
	}


	const VertexLayout* OpenGLGraphicsDevice::GetVertexLayout(VertexLayoutHandle handle) const
	{
		// Kind of a trick : we rely on the fact all valid VAO IDs will always start at 1 and increment by one.
		// So, a valid handle is simply at index (VAO id - 1) in the array.
		// NOTE: this assumption won't stand if we add the ability to remove vertex layouts. So far, a vertex layout gets created "forever" (you cannot delete them).
		// TODO: really have to fix that as it also breaks RenderDoc !

		if (MOE_ASSERT(handle.Get() <= m_vertexLayouts.Size()))
		{
			return &m_vertexLayouts[handle.Get() - 1];
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
		const uint32_t meshOffset = m_vertexBufferPool.Allocate(data, (uint32_t)byteSize);
		if (meshOffset == OpenGLBuddyAllocator::ms_INVALID_OFFSET)
		{
			return DeviceBufferHandle::Null();
		}
		else
		{
			// Encode the VBO ID and the offset in the handle.
			// The handle looks like : | VBO ID (32 bits) | offset in VBO (32 bits) |
			uint64_t handleValue = (uint64_t)m_vertexBufferPool.GetBufferHandle() << 32;
			handleValue |= meshOffset;
			return DeviceBufferHandle{handleValue};
		}
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

			glDrawElements(GL_TRIANGLES, (GLsizei)numIndices, GL_UNSIGNED_INT, (const void*)((uint64_t)eboOffset));
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)numVertices);
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
		const GLuint textureFormat = TranslateToOpenGLSizedFormat(tex2DDesc.m_targetFormat);
		if (textureFormat == 0)
		{
			return Texture2DHandle::Null();
		}

		// we have the data: upload to GPU
		GLuint textureID;

		glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
		glTextureStorage2D(textureID, tex2DDesc.m_wantedMipmapLevels, textureFormat, tex2DDesc.m_width, tex2DDesc.m_height);

		glTextureSubImage2D(textureID, 0, 0, 0, tex2DDesc.m_width, tex2DDesc.m_height, GL_RGBA, GL_UNSIGNED_BYTE, tex2DDesc.m_imageData);

		if (tex2DDesc.m_wantedMipmapLevels != 0)
		{
			glGenerateTextureMipmap(textureID);
		}

		return Texture2DHandle{textureID};
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


	void OpenGLGraphicsDevice::BindTextureUnitToProgramUniform(GLuint shaderProgramID, int textureUnitIndex, Texture2DHandle texHandle, const char* uniformName)
	{
		glBindTextureUnit(textureUnitIndex, texHandle.Get());
		glProgramUniform1i(shaderProgramID, glGetUniformLocation(shaderProgramID, uniformName), textureUnitIndex);
	}


	void OpenGLGraphicsDevice::BindTextureUnit(int textureBindingPoint, Texture2DHandle texHandle)
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