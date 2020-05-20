// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

#include <Core/Containers/HashMap/HashMap.h>

#include <glad/glad.h>

namespace moe
{
	/**
	 * \brief A thin wrapper over an OpenGL shader program in RAII-fashion that automatically deletes itself when going out of scope.
	 * It is handy to avoid forgetting to delete them.
	 */
	class OpenGLShaderProgram
	{
		static const GLuint ms_nullProgram = 0;

	public:

		OpenGLShaderProgram(GLuint shaderProgram) :
			m_program(shaderProgram)
		{}

		// Delete those functions because OpenGLShaderProgram cannot be copied (avoids duplicate glDeleteProgram's)
		OpenGLShaderProgram(const OpenGLShaderProgram &) = delete;
		OpenGLShaderProgram & operator=(const OpenGLShaderProgram &) = delete; // assignment op

		OpenGLShaderProgram(OpenGLShaderProgram&& other) noexcept;

		OpenGLShaderProgram& operator=(OpenGLShaderProgram&& rhs) noexcept;


		~OpenGLShaderProgram()
		{
			if (m_program != ms_nullProgram)
			{
				glDeleteProgram(m_program);
			}
		}


		void	Attach(GLuint shaderId)
		{
			glAttachShader(m_program, shaderId);
		}


		void	Link()
		{
			glLinkProgram(m_program);
		}


		/**
		 * \brief Builds a cache of uniform block binding point -> block index for faster access,
		 * and also the offset of every variable in uniform blocks to make their modification easier.
		 */
		void	BuildUniformBlockAccessCache();


		int		GetBlockMemberOffset(const std::string& memberVariableName) const;


		operator GLuint() const
		{
			return m_program;
		}


		/**
		 * \brief Comparison operator needed to put programs in std::set for example
		 * \param other The other compared shader program
		 * \return
		 */
		bool operator<(const OpenGLShaderProgram & other) const
		{
			return m_program < other.m_program;
		}


	private:
		HashMap<int, unsigned>	m_blockBindingToBlockIdx;
		HashMap<std::string, int>	m_blockMemberNameToOffset;

		GLuint	m_program{ ms_nullProgram };
	};


}

#endif // #ifdef MOE_OPENGL