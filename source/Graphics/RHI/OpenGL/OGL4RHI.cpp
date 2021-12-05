#include "OGL4RHI.h"


namespace moe
{
	OpenGL4RHI::OpenGL4RHI(GLLoaderFunction loaderFunc) :
		m_framebufferManager(m_textureManager),
		m_swapchainManager(m_framebufferManager)
	{
		// Very important : this has to be done and working before any gl* call !
		bool ok = gladLoadGLLoader((GLADloadproc)loaderFunc);
		if (false == ok)
		{
			MOE_FATAL(ChanWindowing, "Failed to initialize OpenGL context");
			MOE_DEBUG_ASSERT(false);
			return;
		}

		// Activate OpenGL debug messages callback
		GLint flags = 0;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLDebugMessageRoutine, this);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE); // we want to log all errors
		}

	}


	void OpenGL4RHI::OpenGLDebugMessageRoutine(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/,
		const char* message, const void* /*userParam*/)
	{
		static const auto ignoredErrorNumbers = std::array<GLuint,4>{ 131169, 131185, 131218, 131204 };
		// ignore non-significant error/warning codes
		if (std::find(ignoredErrorNumbers.begin(), ignoredErrorNumbers.end(), id) != ignoredErrorNumbers.end())
			return;

		std::string errorMessage;
		errorMessage.reserve(128);

		errorMessage = "OpenGL error (ID %u) : %s\n";

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:				errorMessage += "Source: API\n"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:		errorMessage += "Source: Window System\n"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:	errorMessage += "Source: Shader Compiler\n"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:		errorMessage += "Source: Third Party\n"; break;
		case GL_DEBUG_SOURCE_APPLICATION:		errorMessage += "Source: Application\n"; break;
		case GL_DEBUG_SOURCE_OTHER:
		default:
												errorMessage += "Source: Other\n"; break;
		}

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:				errorMessage += "Type: Error\n"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:	errorMessage += "Type: Deprecated Behaviour\n"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:	errorMessage += "Type: Undefined Behaviour\n"; break;
		case GL_DEBUG_TYPE_PORTABILITY:			errorMessage += "Type: Portability\n"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:			errorMessage += "Type: Performance\n"; break;
		case GL_DEBUG_TYPE_MARKER:				errorMessage += "Type: Marker\n"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:			errorMessage += "Type: Push Group\n"; break;
		case GL_DEBUG_TYPE_POP_GROUP:			errorMessage += "Type: Pop Group\n"; break;
		case GL_DEBUG_TYPE_OTHER:
		default:
												errorMessage += "Type: Other\n"; break;
		}

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:			errorMessage += "Severity: high\n\n"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:			errorMessage += "Severity: medium\n\n"; break;
		case GL_DEBUG_SEVERITY_LOW:				errorMessage += "Severity: low\n\n"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:	errorMessage += "Severity: notification\n\n"; break;
		default:								errorMessage += "Severity: unknown\n\n"; break;
		}

		MOE_ERROR(moe::ChanGraphics, errorMessage.c_str(), id, message);
		if (severity == GL_DEBUG_SEVERITY_HIGH)
			MOE_DEBUG_ASSERT(false);
	}
}
