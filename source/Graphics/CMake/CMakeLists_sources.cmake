

set(Monocle_Graphics_SOURCES
	./BlendState/BlendEquation.h
./BlendState/BlendFactor.h
./BlendState/BlendStateDescriptor.cpp
./BlendState/BlendStateDescriptor.h
./BlendState/OpenGL/OpenGLBlendEquation.cpp
./BlendState/OpenGL/OpenGLBlendEquation.h
./BlendState/OpenGL/OpenGLBlendFactor.cpp
./BlendState/OpenGL/OpenGLBlendFactor.h
./DepthStencilState/DepthStencilComparisonFunc.h
./DepthStencilState/DepthStencilStateDescriptor.cpp
./DepthStencilState/DepthStencilStateDescriptor.h
./DepthStencilState/StencilOps.h
./DepthStencilState/StencilOpsDescriptor.h
./Device/GraphicsDevice.h
./Device/OpenGL/OpenGLGraphicsDevice.cpp
./Device/OpenGL/OpenGLGraphicsDevice.h
./DeviceBuffer/BufferDescription.h
./DeviceBuffer/BufferUsage.cpp
./DeviceBuffer/BufferUsage.h
./DeviceBuffer/DeviceBuffer.cpp
./DeviceBuffer/DeviceBuffer.h
./DeviceBuffer/OpenGL/OpenGLDeviceBuffer.cpp
./DeviceBuffer/OpenGL/OpenGLDeviceBuffer.h
./GraphicsAllocator/OpenGL/GLGraphicsAllocator.cpp
./GraphicsAllocator/OpenGL/GLGraphicsAllocator.h
./GraphicsDevice/GraphicsDevice.h
./GraphicsDevice/OpenGLGraphicsDevice.cpp
./GraphicsDevice/OpenGLGraphicsDevice.h
./Handle/ObjectHandle.h
./Mesh/Mesh.h
./OpenGL/moeOpenGL.h
./Pipeline/OpenGL/OpenGLPipeline.cpp
./Pipeline/OpenGL/OpenGLPipeline.h
./Pipeline/PipelineDescriptor.h
./RasterizerState/RasterizerStateDescriptor.h
./Renderer/AbstractRenderer/AbstractRenderer.cpp
./Renderer/AbstractRenderer/AbstractRenderer.h
./Renderer/OpenGL/OpenGLRenderer.cpp
./Renderer/OpenGL/OpenGLRenderer.h
./Renderer/Renderer.h
./Renderer/RendererDescriptor.h
./ResourceFactory/OpenGLResourceFactory/OpenGLResourceFactory.cpp
./ResourceFactory/OpenGLResourceFactory/OpenGLResourceFactory.h
./ResourceFactory/ResourceFactory.h
./Resources/shaders/OpenGL/basic.frag
./Resources/shaders/OpenGL/basic.vert
./Shader/Handle/ShaderHandle.h
./Shader/Manager/OpenGL/OpenGLShaderManager.cpp
./Shader/Manager/OpenGL/OpenGLShaderManager.h
./Shader/Module/ShaderModuleDescriptor.h
./Shader/Program/OpenGL/OpenGLShaderProgram.h
./Shader/Program/OpenGL/OpenGLShaderProgramComparator.h
./Shader/Program/ShaderProgramDescriptor.h
./Shader/ShaderStage/OpenGL/OpenGLShaderStage.cpp
./Shader/ShaderStage/OpenGL/OpenGLShaderStage.h
./Shader/ShaderStage/ShaderStage.cpp
./Shader/ShaderStage/ShaderStage.h
./Texture/Texture.h
./Texture/TextureDescription.h
./Texture/TextureView.h
./Texture/TextureViewDescription.h
./Topology/PrimitiveTopology.h
./VertexBuffer/OpenGL/OpenGLVertexBuffer.cpp
./VertexBuffer/OpenGL/OpenGLVertexBuffer.h
./VertexBuffer/VertexBuffer.cpp
./VertexBuffer/VertexBuffer.h
./VertexLayout/OpenGL/OpenGLVertexFormat.h
./VertexLayout/OpenGL/OpenGLVertexLayout.h
./VertexLayout/VertexElementDescriptor.h
./VertexLayout/VertexElementFormat.h
./VertexLayout/VertexLayout.h
./VertexLayout/VertexLayoutDescriptor.h
./VertexLayout/VertexLayoutHandle.h
	)

if(WIN32)
	set(Monocle_Graphics_SOURCES

	${Monocle_Graphics_SOURCES}

	
	)

elseif(APPLE)
	set(Monocle_Graphics_SOURCES

	${Monocle_Graphics_SOURCES}

	
	)

elseif(UNIX)
	set(Monocle_Graphics_SOURCES

	${Monocle_Graphics_SOURCES}

	
	)


endif()
