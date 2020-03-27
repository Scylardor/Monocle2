// Monocle Game Engine source files - Alexandre Baron

#ifdef MOE_OPENGL

#include "OpenGLRenderer.h"

#include "Graphics/Shader/ShaderStage/OpenGL/OpenGLShaderStage.h"
#include "Graphics/Shader/Program/OpenGL/OpenGLShaderProgram.h"


static const unsigned ms_INFOLOG_BUF_SIZE = 512;

#define GL_SHADER_CHECK_MESSAGE(shaderObj, status, ...) \
{ \
	int success; \
	glGetShaderiv(shaderObj, status, &success); \
	if (false == success) \
	{ \
		char infoLog[ms_INFOLOG_BUF_SIZE]; \
		glGetShaderInfoLog(shaderObj, ms_INFOLOG_BUF_SIZE, nullptr, infoLog); \
		MOE_ERROR(ChanGraphics, ##__VA_ARGS__); \
		return; \
	} \
}


namespace moe
{
	void OpenGLRenderer::CreateShaderProgramFromSource(const ShaderProgramDescriptor& shaProDesc)
	{
		// First create a program
		OpenGLShaderProgram program{ glCreateProgram() };

		// Then attach all our shaders
		for (const ShaderModuleDescriptor& shaderModDesc : shaProDesc)
		{
			const GLenum shaderStageEnum = GetShaderStageEnum(shaderModDesc.m_moduleStage);

			if (shaderStageEnum == GL_FALSE)
			{
				MOE_ERROR(ChanGraphics, "Could not translate shader stage value '%s'. Aborting shader program creation.", shaderModDesc.m_moduleStage._to_string());
				return;
			}

			const GLuint newShader = glCreateShader(shaderStageEnum);

			// Check for shader compile errors
			glCompileShader(newShader);

			GL_SHADER_CHECK_MESSAGE(newShader, GL_COMPILE_STATUS,
				"Compilation failed for shader stage '%s'. Aborting shader program creation.", shaderModDesc.m_moduleStage._to_string());

			program.Attach(newShader);

			// Contrary to popular belief, it is ok to delete shaders straight away, because as long as they are attached to the program, they won't be deleted.
			// However, once the program is destroyed they will be automatically destroyed, because flagged for deletion and the program they were attached to is gone.
			// cf. https://stackoverflow.com/questions/24172962/gldeleteshader-is-the-order-irrelevant
			glDeleteShader(newShader);
		}

		// Finally link shader program
		program.Link();

		// Check for linking errors
		GL_SHADER_CHECK_MESSAGE(program, GL_LINK_STATUS,
			"Linking failed for shader program '%u'. Aborting shader program creation.", (GLuint)program);
	}
}

#endif // MOE_OPENGL