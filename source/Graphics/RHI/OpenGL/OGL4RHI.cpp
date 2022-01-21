#include "OGL4RHI.h"

#include <backends/imgui_impl_opengl3.h>


#include "Graphics/CommandBuffer/CommandBuffer.h"


namespace moe
{
	OpenGL4RHI::OpenGL4RHI(GLLoaderFunction loaderFunc) :
		m_framebufferManager(m_textureManager),
		m_swapchainManager(this),
		m_materialManager(m_textureManager)
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
			DebugSetup();
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


	void OpenGL4RHI::EndRenderPass()
	{
		m_framebufferManager.UnbindFramebuffer();
		glUseProgram(0);
		glBindVertexArray(0);
	}


	void OpenGL4RHI::DebugSetup()
	{
		// List all extensions
		GLint numExts = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &numExts);
		MOE_INFO(ChanGraphics, "List of supported OpenGL extensions (%i):", numExts);
		for (GLint i = 0; i < numExts; i++)
		{
			auto* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
			MOE_INFO(ChanGraphics, "\t%s", extension);
		}

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLDebugMessageRoutine, this);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE); // we want to log all errors
	}


	// Needed to visit variants https://www.bfilipek.com/2018/09/visit-variants.html
	template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
	template<class... Ts> overload(Ts...)->overload<Ts...>;

	void OpenGL4RHI::SubmitCommandBuffer(CommandBuffer const& cmdBuf)
	{
		// Some convenience variables to go faster in the visit hot loop
		uint32_t lastMaterialUsedIdx = INVALID_ID;
		uint32_t lastProgramUsed = 0;
		OpenGL4VertexLayout const* lastLayoutUsed = nullptr;

		for (CommandBufferVariant const& cmd : cmdBuf)
		{
			std::visit(overload{
					[this](CmdBeginRenderPass const & cbrp)
					{
						BeginRenderPass(cbrp);
					},
					[&](CmdBindMaterial const & cbm)
					{
						MOE_DEBUG_ASSERT(cbm.Handle.IsNotNull());
						if (cbm.Handle.IsNull())
							return;

						lastMaterialUsedIdx = cbm.Handle.Get() >> 32;
						lastLayoutUsed = m_materialManager.GetMaterialVertexLayout(lastMaterialUsedIdx);
						lastProgramUsed = m_materialManager.BindMaterial(this, lastMaterialUsedIdx);
					},
					[&](CmdBindResourceSet const& cbrs)
					{
						MOE_DEBUG_ASSERT(cbrs.Handle.IsNotNull());
						if (cbrs.Handle.IsNull())
							return;
						m_materialManager.BindDynamicResourceSets(this, cbrs.Handle.Get(), lastProgramUsed);
					},
					[&](CmdDrawMesh const & cdm)
					{
						if (cdm.DynamicSets != INVALID_ID)
							m_materialManager.BindDynamicResourceSets(this, cdm.DynamicSets, lastProgramUsed);

						m_bufferManager.DrawMesh(cdm.Handle, lastLayoutUsed);
					},
					[this](CmdEndRenderPass const &)
					{
						EndRenderPass();
					},
					[this](CmdPresentSwapchain const & cps)
					{
						m_swapchainManager.Present(cps.Handle);
					},
					[this](CmdSetViewportScissor const& csvs)
					{
						glViewport(csvs.Viewport.x, csvs.Viewport.y, csvs.Viewport.Width, csvs.Viewport.Height);
						glScissor(csvs.Scissor.x, csvs.Scissor.y, csvs.Scissor.Width, csvs.Scissor.Height);
					},
					[this](CmdDrawArrays const& cda)
					{
						auto topo = OpenGLPipeline::GetOpenGLPrimitiveTopology(cda.Topo);
						glDrawArrays(topo, cda.First, cda.Count);
					},
					[](CmdRenderImGuiDrawData const& cridd)
					{
						// I want to keep the variant const, but Imgui expects its draw data non-const.
						// So cast the const away
						ImDrawData* drawData = const_cast<ImDrawData*>(cridd.DrawData);
						ImGui_ImplOpenGL3_RenderDrawData(drawData);
					}
				}, cmd
			);
		}
	}


	void OpenGL4RHI::OpenGLDebugMessageRoutine(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/,
											   const char* message, const void* /*userParam*/)
	{
		static const auto ignoredErrorNumbers = std::array<GLuint,4>{ 131169, 131185, 131218, 131204 };
		// ignore non-significant error/warning codes
		if (std::find(ignoredErrorNumbers.begin(), ignoredErrorNumbers.end(), id) != ignoredErrorNumbers.end())
			return;

		std::string errorMessage = "OpenGL error (ID %u) : %s\n";

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
