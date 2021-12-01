#pragma once

#include <glad/glad.h>

#include "Graphics/RHI/RenderHardwareInterface.h"
#include "BufferManager/BufferManager.h"
#include "PipelineManager/PipelineManager.h"
#include "TextureManager/TextureManager.h"


namespace moe
{
	class OpenGL4RHI : public RenderHardwareInterface
	{
	public:

		using GLLoaderFunction = GLADloadproc;

		OpenGL4RHI(GLLoaderFunction loaderFunc);

		~OpenGL4RHI() override = default;

		IPipelineManager& PipelineManager() override
		{
			return m_pipelineManager;
		}

		IBufferManager& BufferManager() override
		{
			return m_bufferManager;
		}

		ITextureManager&	TextureManager() override
		{
			return m_textureManager;
		}


	private:

		static	void			OpenGLDebugMessageRoutine(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, const void* userParam);

		OpenGL4BufferManager	m_bufferManager;
		OpenGL4PipelineManager	m_pipelineManager;
		OpenGL4TextureManager	m_textureManager;
	};

}
