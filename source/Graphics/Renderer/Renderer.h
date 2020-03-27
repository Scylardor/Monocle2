// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include <Graphics/Shader/Program/ShaderProgramDescriptor.h>

namespace moe
{
	/*
		The base class for all other renderer classes.
		Contains the graphics device necessary to output commands to the GPU.
	*/
	class IGraphicsRenderer
	{
	public:
		virtual ~IGraphicsRenderer() = default;

		virtual void	CreateShaderProgramFromSource(const ShaderProgramDescriptor& shaProDesc) = 0;
		virtual void	CreateShaderProgramFromBinary(const ShaderProgramDescriptor& shaProDesc) = 0;


	};

}