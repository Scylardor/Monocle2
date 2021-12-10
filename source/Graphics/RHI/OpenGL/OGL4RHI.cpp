#include "OGL4RHI.h"

#include "Graphics/CommandBuffer/CommandBuffer.h"


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

	void OpenGL4RHI::BeginRenderPass(CmdBeginRenderPass const& cbrp)
	{
		// OpenGL doesn't have a concept of "swap chain".
		// So if someone tries to bind the fake "swapchain framebuffer", just do nothing :
		// it's going to draw on the default framebuffer.
		if (cbrp.PassFramebuffer != m_swapchainManager.GetMainSwapchainFramebufferHandle())
			m_framebufferManager.BindFramebuffer(cbrp.PassFramebuffer);

		glClearColor(cbrp.ClearColor.R(), cbrp.ClearColor.G(), cbrp.ClearColor.B(), cbrp.ClearColor.A());

		glClearDepth((double) cbrp.DepthClear);
		glClearStencil(cbrp.StencilClear);

		auto clearBits = GL_COLOR_BUFFER_BIT;
		OpenGL4FramebufferDescription const& fbDesc = m_framebufferManager.GetFramebuffer(cbrp.PassFramebuffer);
		if (fbDesc.DepthStencilAttachment.IsNotNull())
		{
			clearBits |= GL_DEPTH_BUFFER_BIT;
			if (FormatHasStencilComponent(m_textureManager.GetTextureData(fbDesc.DepthStencilAttachment).Format))
				clearBits |= GL_STENCIL_BUFFER_BIT;
		}

		glClear(clearBits);
	}


	// Needed to visit variants https://www.bfilipek.com/2018/09/visit-variants.html
	template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
	template<class... Ts> overload(Ts...)->overload<Ts...>;

	void OpenGL4RHI::SubmitCommandBuffer(CommandBuffer const& cmdBuf)
	{

		for (CommandBufferVariant const& cmd : cmdBuf)
		{
			std::visit(overload{
					[this](CmdBeginRenderPass const & cbrp)
					{
						BeginRenderPass(cbrp);
					},
					[this](CmdBindMaterial const &)
					{
						//m_materialManager.CreatePipelineStateObjectLayout()
					},
					[this](CmdDrawMesh const &) {},
					[this](CmdEndRenderPass const &)
					{
						m_framebufferManager.UnbindFramebuffer();
					},
					[this](CmdPresentSwapchain const & cps)
					{
						m_swapchainManager.Present(cps.Handle);
					}
				}, cmd);
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
