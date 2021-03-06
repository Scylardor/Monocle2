// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include <glad/glad.h>

#include "Graphics/Texture/TextureFormat.h"

#include "Graphics/Texture/Texture2DHandle.h"

namespace moe
{
	/**
	 * \brief Takes a graphics-API agnostic texture format and returns the equivalent OpenGL sized internal format enum value.
	 * \param format The format to translate
	 * \return An OpenGL sized enum value matching the requested texture format
	 */
	GLenum	TranslateToOpenGLSizedFormat(TextureFormat format);


	/**
	 * \brief Takes a graphics-API agnostic texture format and returns the equivalent OpenGL base format enum value.
	 * It's almost the same as TranslateToOpenGLSizedFormat, except it's using base format enums.
	 * For example : given RGBA32F, it will return GL_RGBA, not GL_RGBA32F.
	 * \param format The format to translate
	 * \return An OpenGL base enum value matching the requested texture format
	 */
	GLenum	TranslateToOpenGLBaseFormat(TextureFormat format);


	/**
	 * \brief Takes a graphics-API agnostic texture format and returns the equivalent OpenGL base format enum value.
	 * It's almost the same as TranslateToOpenGLBaseFormat, except it's using a number of channels as input.
	 * For example : given 4, it will return GL_RGBA.
	 * \param channelsNbr The number of channels of this format
	 * \return An OpenGL base enum value matching the requested texture format
	 */
	GLenum	TranslateToOpenGLBaseFormat(int channelsNbr);


	GLenum	TranslateToOpenGLTypeEnum(TextureFormat format);



	// TODO: these should probably be elsewhere
	TextureHandle	EncodeRenderbufferHandle(GLuint renderBufferID);
	TextureHandle	DecodeRenderbufferHandle(TextureHandle renderbufferHandle);
	bool			IsARenderBufferHandle(TextureHandle texHandle);

}