
#include "Core/Log/moeLogMacros.h"
#ifdef MOE_OPENGL

#include <glad/glad.h>

#include <STB/stb_image.h>

#include "GL4TextureFactory.h"

#include "Graphics/Texture/OpenGL/OpenGLTextureFormat.h"


namespace moe
{
	GL4TextureFactory::GL4TextureFactory(ResourceManager& rscMgr, bool verticalFlipLoadedTextures) :
		ITextureFactory2(rscMgr), m_verticalFlipLoadedImages(verticalFlipLoadedTextures)
	{
		stbi_set_flip_vertically_on_load(m_verticalFlipLoadedImages);
	}


	GL4TextureFactory::~GL4TextureFactory()
	{
		// The factory is destroyed : destroy every object still alive
		m_TexHandles.ForEach([](GLuint GLHandle)
			{
				if (IsARenderBufferHandle(GLHandle))
				{
					GLHandle = DecodeRenderbufferHandle(GLHandle);
					glDeleteRenderbuffers(1, &GLHandle);
				}
				else
				{
					glDeleteTextures(1, &GLHandle);
				}
			});
	}


	TexHandle GL4TextureFactory::CreateTexture2D(const Tex2DDescriptor& tex2dDesc)
	{
		// First ensure the target texture format is valid - don't bother going further if not
		const GLuint GLtextureFormat = TranslateToOpenGLSizedFormat(tex2dDesc.m_TargetFormat);
		if (GLtextureFormat == 0)
		{
			return TexHandle::Null();
		}

		// we have the data: upload to GPU
		GLuint textureID;

		// If the required texture is a write-only (not Sampled) render target, use a renderbuffer object instead.
		// Renderbuffer objects store all the render data directly into their buffer without any conversions to texture-specific formats.
		// It makes them faster as a writeable storage medium, as they are write-only objects.
		if (tex2dDesc.m_Usage & RenderTarget && (tex2dDesc.m_Usage & Sampled) == 0)
		{
			TexHandle rbHandle = AddRenderbuffer(GLtextureFormat, tex2dDesc.m_Width, tex2dDesc.m_Height);
			return rbHandle;
		}
		else
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
			MOE_DEBUG_ASSERT(textureID < 0x7FFFFFFF); // We use the 32th bit to flag if textures are renderbuffers or not

			glTextureStorage2D(textureID, tex2dDesc.m_MipMaps, GLtextureFormat, tex2dDesc.m_Width, tex2dDesc.m_Height);
			if (tex2dDesc.HasImageData())
			{
				// Analyse the source format enum to know what would be the appropriate input type and format to use.
				auto inputBaseFormat = TranslateToOpenGLBaseFormat(tex2dDesc.m_SourceFormat);
				auto inputFormatTypeEnum = TranslateToOpenGLTypeEnum(tex2dDesc.m_SourceFormat);

				glTextureSubImage2D(textureID, 0, 0, 0, tex2dDesc.m_Width, tex2dDesc.m_Height, inputBaseFormat, inputFormatTypeEnum, tex2dDesc.m_ImageData);
			}

			if (tex2dDesc.m_MipMaps != 0)
			{
				glGenerateTextureMipmap(textureID);
			}

			auto renderObjectID = m_TexHandles.Add(textureID);
			return TexHandle{ textureID, renderObjectID };
		}
	}


	TexHandle GL4TextureFactory::CreateTexture2DFromFile(const Tex2DFileDescriptor& tex2DFileDesc)
	{
		// First verify the target format is correct
		const GLuint textureFormat = TranslateToOpenGLSizedFormat(tex2DFileDesc.m_TargetFormat);
		if (textureFormat == 0)
		{
			return TexHandle::Null();
		}

		// Now read the file, taking into account required number of components.
		const bool isTargetRadianceHDR = IsRadianceHDRFormat(tex2DFileDesc.m_TargetFormat);
		const std::uint32_t requiredCompNum = GetTextureFormatChannelsNumber(tex2DFileDesc.m_RequiredFormat);

		int width, height, nrChannels;
		void* const imageData = isTargetRadianceHDR ?
			(void*)stbi_loadf(tex2DFileDesc.m_Filename.c_str(), &width, &height, &nrChannels, requiredCompNum)
			: (void*)stbi_load(tex2DFileDesc.m_Filename.c_str(), &width, &height, &nrChannels, requiredCompNum);

		if (imageData == nullptr)
		{
			MOE_ERROR(ChanGraphics, "Image file %s could not be read.", tex2DFileDesc.m_Filename);
			MOE_DEBUG_ASSERT(false);
			return TexHandle::Null();
		}

		// we have the data: upload to GPU

		// Determine what could be a good base format.
		const GLuint inputBaseFormat = TranslateToOpenGLBaseFormat(nrChannels);

		const GLenum targetType = (isTargetRadianceHDR ? GL_FLOAT : GL_UNSIGNED_BYTE);

		GLuint textureID;

		glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
		MOE_DEBUG_ASSERT(textureID < 0x7FFFFFFF); // We use the 32th bit to flag if textures are renderbuffers or not

		glTextureStorage2D(textureID, tex2DFileDesc.m_MipMaps, textureFormat, width, height);
		glTextureSubImage2D(textureID, 0, 0, 0, width, height, inputBaseFormat, targetType, imageData);

		// we don't need to keep the image data
		stbi_image_free(imageData);

		if (tex2DFileDesc.m_MipMaps != 0)
		{
			glGenerateTextureMipmap(textureID);
		}

		auto renderObjectID = m_TexHandles.Add(textureID);
		return TexHandle{ textureID, renderObjectID };
	}


	TexHandle GL4TextureFactory::CreateCubemapTexture(const CubemapTexDescriptor& cubeFaceTexDesc)
	{
		// First ensure source and target texture formats are valid - don't bother going further if not
		const GLuint sourceFormat = cubeFaceTexDesc.HasImageData() ? TranslateToOpenGLSizedFormat(cubeFaceTexDesc.m_SourceFormat) : 0;
		const GLuint storageFormat = TranslateToOpenGLSizedFormat(cubeFaceTexDesc.m_TargetFormat);
		if ((sourceFormat == 0 && cubeFaceTexDesc.HasImageData()) || storageFormat == 0)
		{
			MOE_DEBUG_ASSERT(false); // not supposed to happen
			return TexHandle::Null();
		}

		// Creating a cube map with DSA requires us to use glTextureStorage2D + glTextureSubImage3D.
		// For more info, see : https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions#uploading-cube-maps
		// Or : https://www.reddit.com/r/opengl/comments/556zac/how_to_create_cubemap_with_direct_state_access/
		GLuint cubemapID;
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &cubemapID);
		MOE_DEBUG_ASSERT(cubemapID < 0x7FFFFFFF); // We use the 32th bit to flag if textures are renderbuffers or not
		glTextureStorage2D(cubemapID, cubeFaceTexDesc.m_MipMaps, storageFormat, cubeFaceTexDesc.m_Width, cubeFaceTexDesc.m_Height);

		// The data upload assumes the 6 images are stored contiguously in memory at address imageData.
		// Note : we cannot pass a nullptr to glTextureSubImage3D.
		if (cubeFaceTexDesc.HasImageData())
		{
			const uint8_t nbrComponents = GetTextureFormatChannelsNumber(cubeFaceTexDesc.m_SourceFormat);
			const unsigned imageByteOffset = cubeFaceTexDesc.m_Width * cubeFaceTexDesc.m_Height * nbrComponents;

			for (GLint face = 0; face < 6; ++face)
			{
				const void* imagePtr = cubeFaceTexDesc.m_ImageData + (face * imageByteOffset);
				glTextureSubImage3D(cubemapID, 0, 0, 0, face, cubeFaceTexDesc.m_Width, cubeFaceTexDesc.m_Height, 1, sourceFormat, GL_UNSIGNED_BYTE, imagePtr);
			}

			if (cubeFaceTexDesc.m_MipMaps != 0)
			{
				glGenerateTextureMipmap(cubemapID);
			}
		}

		auto renderObjectID = m_TexHandles.Add(cubemapID);
		return TexHandle{ cubemapID, renderObjectID };
	}


	TexHandle GL4TextureFactory::CreateCubemapTextureFromFile(const CubemapTexFilesDescriptor& cubemapFilesDesc)
	{
		// First ensure the target texture format is valid - don't bother going further if not
		const GLuint textureFormat = TranslateToOpenGLSizedFormat(cubemapFilesDesc.m_TargetFormat);
		if (textureFormat == 0)
		{
			MOE_DEBUG_ASSERT(false); // not supposed to happen
			return TexHandle::Null();
		}

		// Now read the files, taking into account required number of components.
		const std::uint32_t requiredCompNum = GetTextureFormatChannelsNumber(cubemapFilesDesc.m_RequiredFormat);

		unsigned char* imagesData[6]{ nullptr };
		int iCubeSide = 0;
		int cubemapWidth, cubemapHeight, cubemapNrChannels;
		cubemapWidth = cubemapHeight = cubemapNrChannels = 0;

		bool failed = false; // start optimistic

		// contrary to the usual OpenGL behaviour of having the image origin in the lower left,
		// Cube Maps' specification is that the images' origin is in the upper left.
		// So specifically when loading cube maps, we have to revert our vertical flipping image loading logic.
		// cf. https://stackoverflow.com/questions/11685608/convention-of-faces-in-opengl-cubemapping
		stbi_set_flip_vertically_on_load(false);

		for (const auto& fileName : cubemapFilesDesc.m_RightLeftTopBottomFrontBackTexFiles)
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

		// Turn the flip mode back to whatever it was
		stbi_set_flip_vertically_on_load(m_verticalFlipLoadedImages);

		if (!MOE_ASSERT(!failed))
		{
			// Something went wrong: free the data and stop here
			for (unsigned char* imgData : imagesData)
			{
				if (imgData != nullptr)
					stbi_image_free(imgData);
			}
			return TexHandle::Null();
		}

		// From now on we successfully loaded image data : upload to GPU

		// Determine what could be a good base format
		const GLuint inputBaseFormat = TranslateToOpenGLBaseFormat(cubemapNrChannels);

		// Creating a cube map with DSA requires us to use glTextureStorage2D + glTextureSubImage3D.
		// For more info, see : https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions#uploading-cube-maps
		// Or : https://www.reddit.com/r/opengl/comments/556zac/how_to_create_cubemap_with_direct_state_access/
		GLuint cubemapID;
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &cubemapID);
		MOE_DEBUG_ASSERT(cubemapID < 0x7FFFFFFF); // We use the 32th bit to flag if textures are renderbuffers or not

		glTextureStorage2D(cubemapID, cubemapFilesDesc.m_MipMaps, textureFormat, cubemapWidth, cubemapHeight);
		for (GLint face = 0; face < 6; ++face)
		{
			glTextureSubImage3D(cubemapID, 0, 0, 0, face, cubemapWidth, cubemapHeight, 1, inputBaseFormat, GL_UNSIGNED_BYTE, imagesData[face]);
		}

		if (cubemapFilesDesc.m_MipMaps != 0)
		{
			glGenerateTextureMipmap(cubemapID);
		}

		// we don't need to keep the image data
		for (unsigned char* imgData : imagesData)
		{
			if (imgData != nullptr)
				stbi_image_free(imgData);
		}

		auto renderObjectID = m_TexHandles.Add(cubemapID);
		return TexHandle{ cubemapID, renderObjectID };
	}


	void GL4TextureFactory::DeleteTexture(TexHandle deletedTexture)
	{
		// First remove it from the texture list
		m_TexHandles.Remove(deletedTexture.RenderObjectID);

		// Now let OpenGL free it
		auto glHandle = deletedTexture.Value;
		if (IsARenderBufferHandle(glHandle))
		{
			glHandle = DecodeRenderbufferHandle(glHandle);
			glDeleteRenderbuffers(1, &glHandle);
		}
		else
		{
			glDeleteTextures(1, &glHandle);
		}
	}


	TexHandle GL4TextureFactory::AddRenderbuffer(GLuint GLtextureFormat, uint32_t width, uint32_t height)
	{
		unsigned int rbo;
		glCreateRenderbuffers(1, &rbo);
		glNamedRenderbufferStorage(rbo, GLtextureFormat, width, height);
		rbo = EncodeRenderbufferHandle(rbo);

		auto renderObjectID = m_TexHandles.Add(rbo);

		return TexHandle{rbo, renderObjectID};
	}


	GLuint GL4TextureFactory::EncodeRenderbufferHandle(GLuint renderBufferID)
	{
		MOE_ASSERT((renderBufferID & (1u << 31u)) == false);
		renderBufferID |= (1u << 31u);
		return renderBufferID;
	}


	GLuint GL4TextureFactory::DecodeRenderbufferHandle(GLuint renderbufferHandle)
	{
		// Just return the handle value with the last bit masked out.
		return renderbufferHandle & ~(1u << 31u);
	}


	bool GL4TextureFactory::IsARenderBufferHandle(GLuint texHandle)
	{
		return (texHandle & (1u << 31u));
	}

}

#endif