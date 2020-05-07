// Monocle Game Engine source files - Alexandre Baron

#ifdef MOE_OPENGL

#include "OpenGLRenderer.h"

#include <glad/glad.h>

#include "Graphics/VertexLayout/OpenGL/OpenGLVertexFormat.h"


namespace moe
{
	bool OpenGLRenderer::Initialize(IGraphicsRenderer::GraphicsContextSetup setupFunction)
	{
		// Very important to do this call before any OpenGL function is used !
		if (false == gladLoadGLLoader((GLADloadproc)setupFunction))
		{
			return false;
		}

		// Activate OpenGL debug messages callback
		GLint flags = 0;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLRenderer::OpenGLDebugMessageRoutine, this);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE); // we want to log all errors
		}

		// Now we can start initialization
		m_device.Initialize();

		return true;
	}


	void OpenGLRenderer::Shutdown()
	{
		m_device.Destroy();
	}


	MeshHandle OpenGLRenderer::CreateStaticMeshFromBuffer(const MeshDataDescriptor& vertexData, const MeshDataDescriptor& indexData)
	{
		if (vertexData.IsNull())
		{
			// No mesh data to process
			return MeshHandle::Null();
		}

		VertexBufferHandle vertexHandle = m_device.CreateStaticVertexBuffer(vertexData.m_dataBuffer, vertexData.m_bufferSizeBytes);
		if (false == MOE_ASSERT(vertexHandle.IsNotNull()))
		{
			return MeshHandle::Null();
		}

		IndexBufferHandle indexHandle = IndexBufferHandle::Null();

		if (false == indexData.IsNull())
		{
			indexHandle = m_device.CreateIndexBuffer(indexData.m_dataBuffer, indexData.m_bufferSizeBytes);
			if (false == MOE_ASSERT(indexHandle.IsNotNull()))
			{
				// If creating the index buffer failed, don't forget to destroy the vertex buffer or it will go dangling
				m_device.DeleteStaticVertexBuffer(vertexHandle);
				return MeshHandle::Null();
			}
		}

		FreelistID newMeshID = m_meshFreelist.Add(vertexHandle, vertexData.m_bufferNumElems, indexHandle, indexData.m_bufferNumElems);

		return MeshHandle{ newMeshID.Index() + 1 }; // +1 to avoid forming a null handle !
	}


	void OpenGLRenderer::DeleteStaticMesh(MeshHandle handle)
	{

		MOE_DEBUG_ASSERT(handle.IsNotNull()); // you're not supposed to pass null handles to this function
		if (handle.IsNull())
			return;

		Mesh& mesh = MutMesh(handle);

		m_device.DeleteStaticVertexBuffer(mesh.GetVertexBufferHandle());

		if (mesh.GetIndexBufferHandle().IsNotNull())
		{
			m_device.DeleteIndexBuffer(mesh.GetIndexBufferHandle());
		}

		m_meshFreelist.Remove(handle.Get() - 1); // - 1 to get back the original index !
	}


	void OpenGLRenderer::UseCamera(CameraHandle camHandle)
	{
		// First fetch associated viewport
		const ACamera& cam = m_cameraManager.GetCamera(camHandle);

		auto vpHandle = cam.GetViewportHandle();

		m_device.UseViewport(vpHandle);

		// For now, the camera is actually not used...
	}


	void OpenGLRenderer::Clear(const ColorRGBAf& clearColor)
	{
		glClearColor(clearColor.R(), clearColor.G(), clearColor.B(), clearColor.A());
		glClear(GL_COLOR_BUFFER_BIT);
	}


	void OpenGLRenderer::UseMaterial(ShaderProgramHandle progHandle)
	{
		m_device.UseShaderProgram(progHandle);

	}


	void OpenGLRenderer::DrawMesh(MeshHandle meshHandle, VertexLayoutHandle layoutHandle)
	{

		Mesh& drawnMesh = MutMesh(meshHandle);

		const OpenGLVertexLayout* vtxLayout = m_device.UseVertexLayout(layoutHandle);
		if (vtxLayout == nullptr)
		{
			return; // TODO : add log
		}

		auto [vbo, vboOffset] = OpenGLGraphicsDevice::DecodeBufferHandle(drawnMesh.GetVertexBufferHandle());

		if (vtxLayout->IsInterleaved())
		{
			glVertexArrayVertexBuffer(vtxLayout->VAO(), 0, vbo, vboOffset, vtxLayout->GetStrideBytes());
		}
		else
		{
			// In packed mode, set the bindings one by one.

			const VertexLayoutDescriptor& layoutDesc = vtxLayout->ReadDescriptor();

			uint32_t bindingIdx = 0;
			size_t elemBufferOffset = vboOffset;

			for (const VertexElementDescriptor& elemDesc : layoutDesc)
			{
				auto oglElemFormat = OpenGLVertexElementFormat::TranslateFormat(elemDesc.m_format);
				auto typeSize = OpenGLVertexElementFormat::FindTypeSize(oglElemFormat.value().m_numCpnts, oglElemFormat.value().m_type);
				glVertexArrayVertexBuffer(vtxLayout->VAO(), bindingIdx, vbo, elemBufferOffset, typeSize.value());

				elemBufferOffset += drawnMesh.NumVertices() * typeSize.value();
				bindingIdx++;
			}
		}

		if (drawnMesh.IsIndexed())
		{
			auto[ebo, eboOffset] = OpenGLGraphicsDevice::DecodeBufferHandle(drawnMesh.GetIndexBufferHandle());

			glVertexArrayElementBuffer(vtxLayout->VAO(), ebo);

			glDrawElements(GL_TRIANGLES, (GLsizei)drawnMesh.NumIndices(), GL_UNSIGNED_INT, (const void*)((uint64_t)eboOffset));
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)drawnMesh.NumVertices());
		}
	}


	// Logic mostly courtesy of https://learnopengl.com/In-Practice/Debugging
	void OpenGLRenderer::OpenGLDebugMessageRoutine(GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei /*length*/, const GLchar* message, const void* /*userParam*/)
	{
		// ignore non-significant error/warning codes
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
			return;

		std::string errorMessage;
		errorMessage.reserve(128);

		errorMessage = "OpenGL error (ID %u) : %s\n";

		switch (source)
		{
			case GL_DEBUG_SOURCE_API:             errorMessage += "Source: API\n"; break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   errorMessage += "Source: Window System\n"; break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: errorMessage += "Source: Shader Compiler\n"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:     errorMessage += "Source: Third Party\n"; break;
			case GL_DEBUG_SOURCE_APPLICATION:     errorMessage += "Source: Application\n"; break;
			case GL_DEBUG_SOURCE_OTHER:           errorMessage += "Source: Other\n"; break;
		}

		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR:               errorMessage += "Type: Error\n"; break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: errorMessage += "Type: Deprecated Behaviour\n"; break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  errorMessage += "Type: Undefined Behaviour\n"; break;
			case GL_DEBUG_TYPE_PORTABILITY:         errorMessage += "Type: Portability\n"; break;
			case GL_DEBUG_TYPE_PERFORMANCE:         errorMessage += "Type: Performance\n"; break;
			case GL_DEBUG_TYPE_MARKER:              errorMessage += "Type: Marker\n"; break;
			case GL_DEBUG_TYPE_PUSH_GROUP:          errorMessage += "Type: Push Group\n"; break;
			case GL_DEBUG_TYPE_POP_GROUP:           errorMessage += "Type: Pop Group\n"; break;
			case GL_DEBUG_TYPE_OTHER:               errorMessage += "Type: Other\n"; break;
		}

		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:         errorMessage += "Severity: high\n\n"; break;
			case GL_DEBUG_SEVERITY_MEDIUM:       errorMessage += "Severity: medium\n\n"; break;
			case GL_DEBUG_SEVERITY_LOW:          errorMessage += "Severity: low\n\n"; break;
			case GL_DEBUG_SEVERITY_NOTIFICATION: errorMessage += "Severity: notification\n\n"; break;
		}

		MOE_ERROR(moe::ChanGraphics, errorMessage.c_str(), id, message);
	}
}

#endif // MOE_OPENGL