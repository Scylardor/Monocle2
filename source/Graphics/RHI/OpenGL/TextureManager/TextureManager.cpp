#include "TextureManager.h"

#include <glad/glad.h>

#include "Graphics/Texture/OpenGL/OpenGLTextureFormat.h"


namespace moe
{
	 DeviceTextureHandle OpenGL4TextureManager::CreateTexture2DFromFile(Ref<FileResource> const& /*textureFile*/)
	{
		 return DeviceTextureHandle::Null();
	}

	 DeviceTextureHandle OpenGL4TextureManager::CreateTexture2D(TextureData const& data)
	{
		const GLuint textureFormat = TranslateToOpenGLSizedFormat(data.Format);
		if (textureFormat == 0)
		{
		 return DeviceTextureHandle::Null();
		}

		// If the required texture is a write-only (not Sampled) render target, use a renderbuffer object instead.
		// Renderbuffer objects store all the render data directly into their buffer without any conversions to texture-specific formats.
		// It makes them faster as a writeable storage medium, as they are write-only objects.
		if ((data.Usage & Sampled) == 0)
		{
			unsigned int rbo;
			glCreateRenderbuffers(1, &rbo);
			glNamedRenderbufferStorage(rbo, textureFormat, data.Width, data.Height);

			auto texHandle = m_textures.Emplace(data, rbo);
			return DeviceTextureHandle(texHandle);
		}

		GLuint textureID;
		glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
		glTextureStorage2D(textureID, data.Mipmaps, textureFormat, data.Width, data.Height);

		// Determine what could be a good base format
		// TODO : Maybe refactor that in another way ? is the GetTextureFormatChannelsNumber really necessary ?
		const std::uint32_t numChannels = GetTextureFormatChannelsNumber(data.Format);
		const GLuint inputBaseFormat = TranslateToOpenGLBaseFormat(numChannels);

		const bool isRadianceHDRFormat = (data.Format == TextureFormat::RGBE);
		const GLenum targetType = (isRadianceHDRFormat ? GL_FLOAT : GL_UNSIGNED_BYTE);

		glTextureSubImage2D(textureID, 0, 0, 0, data.Width, data.Height, inputBaseFormat, targetType, data.Image);

		if (data.Mipmaps != 0)
		{
			 glGenerateTextureMipmap(textureID);
		}

		auto texHandle = m_textures.Emplace(data, textureID);

		return DeviceTextureHandle(texHandle);
	}
}
