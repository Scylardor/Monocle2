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
	 	// TODO: should be stored in the OpenGL4TextureData.
		const GLuint textureFormat = TranslateToOpenGLSizedFormat(data.Format);
		if (textureFormat == 0)
		{
			return DeviceTextureHandle::Null();
		}

		// If the required texture is a write-only (not Sampled) render target, use a renderbuffer object instead.
		// Renderbuffer objects store all the render data directly into their buffer without any conversions to texture-specific formats.
		// It makes them faster as a writeable storage medium, as they are write-only objects.
		if (!data.IsSampledTexture())
		{
			unsigned int rbo;
			glCreateRenderbuffers(1, &rbo);
			glNamedRenderbufferStorage(rbo, textureFormat, data.Width, data.Height);

			auto texHandle = m_textures.Emplace(data, rbo);
			return DeviceTextureHandle(texHandle);
		}

		GLuint textureID;
		glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

		GLsizei mipmapLevels = data.Mipmaps;
	 	// If the user specified "generate as many mipmaps as possible", please do so
		// TODO: this should be done at the texture data creation time...
		if (mipmapLevels == TextureData::MAX_MIPMAPS)
		{
			mipmapLevels = (GLsizei)floor(log2(std::max(data.Width, data.Height)));
			if (mipmapLevels == 0)
				mipmapLevels = 1;
		}

		glTextureStorage2D(textureID, mipmapLevels, textureFormat, data.Width, data.Height);

	 	if (data.Image != nullptr)
	 	{
			// Determine what could be a good base format
			// TODO : Maybe refactor that in another way ? is the GetTextureFormatChannelsNumber really necessary ?
			const std::uint32_t numChannels = GetTextureFormatChannelsNumber(data.Format);
			const GLuint inputBaseFormat = TranslateToOpenGLBaseFormat(numChannels);

			const bool isRadianceHDRFormat = (data.Format == TextureFormat::RGBE);
			const GLenum targetType = (isRadianceHDRFormat ? GL_FLOAT : GL_UNSIGNED_BYTE);

			glTextureSubImage2D(textureID, 0, 0, 0, data.Width, data.Height, inputBaseFormat, targetType, data.Image);
	 	}

		if (mipmapLevels > 1)
		{
			 glGenerateTextureMipmap(textureID);
		}

		auto texHandle = m_textures.Emplace(data, textureID);
		m_textures.Mut(texHandle).Mipmaps = mipmapLevels; // TODO: this should be done at the texture data creation time...

		return DeviceTextureHandle(texHandle);
	}


	void OpenGL4TextureManager::FreeUnderlyingTexture2D(DeviceTextureHandle tex)
	{
		OpenGL4TextureData& texData = m_textures.Mut((uint32_t) tex.Get());

		FreeUnderlyingTexture2D(texData);
	}


	void OpenGL4TextureManager::ResizeTexture2D(DeviceTextureHandle tex, std::pair<int, int> const& newDimensions)
	{
		OpenGL4TextureData& texData = m_textures.Mut((uint32_t)tex.Get());

		FreeUnderlyingTexture2D(texData);

		texData.Width = newDimensions.first;
		texData.Height = newDimensions.second;

		const GLuint textureFormat = TranslateToOpenGLSizedFormat(texData.Format);

		if (!texData.IsSampledTexture())
		{
			unsigned int rbo;
			glCreateRenderbuffers(1, &rbo);
			glNamedRenderbufferStorage(rbo, textureFormat, texData.Width, texData.Height);

			return;
		}

		GLsizei mipmapLevels = texData.Mipmaps;
		// If the user specified "generate as many mipmaps as possible", please do so
		// TODO: this should be done at the texture data creation time...
		if (mipmapLevels == TextureData::MAX_MIPMAPS)
		{
			mipmapLevels = (GLsizei)floor(log2(std::max(texData.Width, texData.Height)));
			if (mipmapLevels == 0)
				mipmapLevels = 1;
		}

		GLuint textureID;
		glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

	 	glTextureStorage2D(textureID, mipmapLevels, textureFormat, texData.Width, texData.Height);

		if (mipmapLevels > 1)
		{
			glGenerateTextureMipmap(textureID);
		}
	}


	 void OpenGL4TextureManager::DestroyTexture2D(DeviceTextureHandle tex)
	 {
		 OpenGL4TextureData const& texData = m_textures.Get((uint32_t)tex.Get());
		 glDeleteTextures(1, &texData.TextureID);
		 m_textures.Free((uint32_t)tex.Get());
	 }


	 void OpenGL4TextureManager::FreeUnderlyingTexture2D(OpenGL4TextureData& texData)
	 {
		 if (texData.IsSampledTexture())
		 {
			 glDeleteTextures(1, &texData.TextureID);
		 }
		 else
		 {
			 glDeleteRenderbuffers(1, &texData.TextureID);
		 }
	 }
}
