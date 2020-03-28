// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/Shader/Program/ShaderProgramDescriptor.h"

#include "Graphics/Shader/Program/OpenGL/OpenGLShaderProgram.h"

#include "Graphics/Shader/Handle/ShaderHandle.h"

#include "Graphics/Shader/Program/OpenGL/OpenGLShaderProgramComparator.h" // for std::set

#ifdef MOE_STD_SUPPORT
	#include <set>
#endif// MOE_STD_SUPPORT


namespace moe
{


	/**
	 * \brief A class that manages OpenGL shader programs.
	 * You pass it compiled shader programs, and it stores them for you.
	 * When you register a new shader, it hands you out a shader handle that you can reuse to get back to that shader.
	 * It technically takes ownership of your shader program. When the manager is destroyed, all the shader programs inside get destroyed.
	 */
	class OpenGLShaderManager
	{

	public:

		/**
		* \brief Creates an OpenGL shader program compiled at runtime, from a descriptor containing GLSL source code.
		* Functions involved : glShaderSource, glCompileShader
		* \param shaProDesc a collection of shader module descriptions
		* \return A valid handle if the shader program was successfully created or ShaderProgramHandle::Null() if it didn't
		*/
		ShaderProgramHandle	CreateShaderProgramFromSource(const ShaderProgramDescriptor& shaProDesc);


		/**
		 * \brief Creates a precompiled OpenGL shader program, from a descriptor containing SPIRV binary code.
		 * Functions involved : glShaderBinary, glSpecializeShader
		 * \param shaProDesc a collection of shader module descriptions
		* \return True if the shader program was successfully created
		 */
		ShaderProgramHandle	CreateShaderProgramFromBinary(const ShaderProgramDescriptor& shaProDesc);

		/**
		 * \brief Stores the provided shader program inside the shader manager.
		 * \param shader
		 * \return
		 */
		ShaderProgramHandle			RegisterProgram(OpenGLShaderProgram&& shader);


		/**
		 * \brief Tries to destroy the shader program referenced by the given handle.
		 * \param handle The handle uniquely identifying the shader program you want to destroy
		 * \return True if the program was successfully destroyed. False if eg. this handle didn't match any existing program
		 */
		bool						DestroyProgram(ShaderProgramHandle handle);


		/**
		 * \brief Retrieves an internal reference to the actual shader program.
		 * \param handle The handle uniquely identifying the shader program you want to destroy
		 * \return A pointer to the referenced OpenGL shader program, or null if this handle didn't match any existing program
		 */
		const OpenGLShaderProgram*	GetProgram(ShaderProgramHandle handle) const;

	private:
		// TODO: I suspect OpenGL always hands you shader program IDs starting from 0,
		// so we could maybe use a more optimized data structure like a freelist to speed things up.
		std::set< OpenGLShaderProgram, OpenGLShaderProgramComparator >	m_programs;


	};

}