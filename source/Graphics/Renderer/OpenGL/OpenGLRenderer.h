// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

#include <Graphics/Renderer/Renderer.h>

#include <glad/glad.h>

namespace moe
{

	class OpenGLRenderer : public IGraphicsRenderer
	{
	public:
		void	CreateShaderProgramFromSource(const ShaderProgramDescriptor& shaProDesc) override;
		void	CreateShaderProgramFromBinary(const ShaderProgramDescriptor& shaProDesc) override;
	};

}

#endif // #ifdef MOE_OPENGL