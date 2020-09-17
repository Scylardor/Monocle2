#pragma once

#include "TextureResource.h"

#include "glad/glad.h"



class OGLTextureResource : public TextureResource
{
public:

	OGLTextureResource(const char * pixels)


private:

	GLuint	m_textureID;

};