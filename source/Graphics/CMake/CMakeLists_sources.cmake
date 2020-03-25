

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
./Mesh/Mesh.h
./Pipeline/OpenGL/OpenGLPipeline.cpp
./Pipeline/OpenGL/OpenGLPipeline.h
./Pipeline/PipelineDescriptor.h
./RasterizerState/RasterizerStateDescriptor.h
./Renderer/OpenGL/OpenGLRenderer.h
./Renderer/Renderer.h
./Renderer/RendererDescriptor.h
./ResourceFactory/OpenGLResourceFactory/OpenGLResourceFactory.cpp
./ResourceFactory/OpenGLResourceFactory/OpenGLResourceFactory.h
./ResourceFactory/ResourceFactory.h
./Texture/Texture.h
./Texture/TextureDescription.h
./Texture/TextureView.h
./Texture/TextureViewDescription.h
./Topology/PrimitiveTopology.h
./VertexBuffer/OpenGL/OpenGLVertexBuffer.cpp
./VertexBuffer/OpenGL/OpenGLVertexBuffer.h
./VertexBuffer/VertexBuffer.cpp
./VertexBuffer/VertexBuffer.h
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
