// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_OPENGL

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

		OpenGLShaderProgram(OpenGLShaderProgram&& other)
		{
			m_program = other.m_program;
			other.m_program = ms_nullProgram;
		}

		OpenGLShaderProgram&	operator=(OpenGLShaderProgram&& rhs)
		{
			if (&rhs != this)
			{
				m_program = rhs.m_program;
				rhs.m_program = ms_nullProgram;
			}

			return *this;
		}


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
		GLuint	m_program{ ms_nullProgram };
	};


}

#endif // #ifdef MOE_OPENGL