

set(Monocle_Graphics_SOURCES
	./BlendState/BlendEquation.h
./BlendState/BlendFactor.h
./BlendState/BlendStateDescriptor.cpp
./BlendState/BlendStateDescriptor.h
./BlendState/OpenGL/OpenGLBlendEquation.cpp
./BlendState/OpenGL/OpenGLBlendEquation.h
./BlendState/OpenGL/OpenGLBlendFactor.cpp
./BlendState/OpenGL/OpenGLBlendFactor.h
./Camera/Camera.cpp
./Camera/Camera.h
./Camera/CameraHandle.h
./Camera/CameraManager.cpp
./Camera/CameraManager.h
./Camera/CameraSystem.cpp
./Camera/CameraSystem.h
./Camera/OpenGL/OpenGLCamera.h
./Camera/Viewport.h
./Camera/ViewportDescriptor.h
./Camera/ViewportHandle.h
./Color/Color.cpp
./Color/Color.h
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
./DeviceBuffer/DeviceBufferHandle.h
./DeviceBuffer/IndexBufferHandle.h
./DeviceBuffer/OpenGL/OpenGLDeviceBuffer.cpp
./DeviceBuffer/OpenGL/OpenGLDeviceBuffer.h
./DeviceBuffer/OpenGL/OpenGLDeviceBufferRange.h
./DeviceBuffer/UniformBufferHandle.h
./DeviceBuffer/VertexBufferHandle.h
./Framebuffer/Framebuffer.h
./Framebuffer/FramebufferAttachments.h
./Framebuffer/FramebufferDescription.h
./Framebuffer/FramebufferHandle.h
./Framebuffer/OpenGL/OpenGLFramebuffer.cpp
./Framebuffer/OpenGL/OpenGLFramebuffer.h
./GraphicsAllocator/OpenGL/OpenGLBuddyAllocator.cpp
./GraphicsAllocator/OpenGL/OpenGLBuddyAllocator.h
./Handle/ObjectHandle.h
./Light/LightObject.cpp
./Light/LightObject.h
./Light/LightSystem.cpp
./Light/LightSystem.h
./Material/Material.cpp
./Material/Material.h
./Material/MaterialBindings.h
./Material/MaterialBlock.h
./Material/MaterialDescriptor.h
./Material/MaterialFrameBlock.h
./Material/MaterialInstance.cpp
./Material/MaterialInstance.h
./Material/MaterialInterface.h
./Material/MaterialLibrary.cpp
./Material/MaterialLibrary.h
./Material/MaterialObjectBlock.h
./Mesh/InstancedMesh.cpp
./Mesh/InstancedMesh.h
./Mesh/Mesh.cpp
./Mesh/Mesh.h
./Mesh/MeshDataDescriptor.h
./Mesh/MeshHandle.h
./Mesh/OpenGL/OpenGLMesh.h
./OpenGL/moeOpenGL.h
./OpenGL/Std140.h
./Pipeline/OpenGL/OpenGLPipeline.cpp
./Pipeline/OpenGL/OpenGLPipeline.h
./Pipeline/PipelineDescriptor.h
./Pipeline/PipelineHandle.h
./RasterizerState/RasterizerStateDescriptor.h
./Renderer/AbstractRenderer/AbstractRenderer.cpp
./Renderer/AbstractRenderer/AbstractRenderer.h
./Renderer/OpenGL/OpenGLRenderer.cpp
./Renderer/OpenGL/OpenGLRenderer.h
./Renderer/Renderer.h
./Renderer/RendererDescriptor.h
./RenderTarget/RenderTargetHandle.h
./RenderWorld/GraphicsObject.cpp
./RenderWorld/GraphicsObject.h
./RenderWorld/GraphicsObjectData.h
./RenderWorld/RenderWorld.cpp
./RenderWorld/RenderWorld.h
./RenderWorld/RenderWorldHandle.h
./ResourceFactory/OpenGLResourceFactory/OpenGLResourceFactory.cpp
./ResourceFactory/OpenGLResourceFactory/OpenGLResourceFactory.h
./ResourceFactory/ResourceFactory.h
./Resources/ResourceLayout/ResourceKind.h
./Resources/ResourceLayout/ResourceLayoutBindingDescriptor.h
./Resources/ResourceLayout/ResourceLayoutDescriptor.h
./Resources/ResourceLayout/ResourceLayoutHandle.h
./Resources/ResourceSet/ResourceHandle.h
./Resources/ResourceSet/ResourceSetDescriptor.h
./Resources/ResourceSet/ResourceSetHandle.h
./Resources/shaders/OpenGL/basic.frag
./Resources/shaders/OpenGL/basic.vert
./Resources/shaders/OpenGL/blending.frag
./Resources/shaders/OpenGL/blending.vert
./Resources/shaders/OpenGL/blinn_phong.frag
./Resources/shaders/OpenGL/blinn_phong.vert
./Resources/shaders/OpenGL/cubemaps.frag
./Resources/shaders/OpenGL/cubemaps.vert
./Resources/shaders/OpenGL/depth_testing.frag
./Resources/shaders/OpenGL/depth_testing.vert
./Resources/shaders/OpenGL/explode.frag
./Resources/shaders/OpenGL/explode.geom
./Resources/shaders/OpenGL/explode.vert
./Resources/shaders/OpenGL/framebuffer.frag
./Resources/shaders/OpenGL/framebuffer.vert
./Resources/shaders/OpenGL/gamma_correction.frag
./Resources/shaders/OpenGL/gamma_correction.vert
./Resources/shaders/OpenGL/geometry_shader.frag
./Resources/shaders/OpenGL/geometry_shader.geom
./Resources/shaders/OpenGL/geometry_shader.vert
./Resources/shaders/OpenGL/instancing.frag
./Resources/shaders/OpenGL/instancing.vert
./Resources/shaders/OpenGL/light.frag
./Resources/shaders/OpenGL/light.vert
./Resources/shaders/OpenGL/phong.frag
./Resources/shaders/OpenGL/phong.vert
./Resources/shaders/OpenGL/phong_maps.frag
./Resources/shaders/OpenGL/phong_maps.vert
./Resources/shaders/OpenGL/skybox.frag
./Resources/shaders/OpenGL/skybox.vert
./Resources/shaders/OpenGL/stencil_testing.frag
./Resources/shaders/OpenGL/stencil_testing.vert
./Resources/shaders/OpenGL/visualize_normals.frag
./Resources/shaders/OpenGL/visualize_normals.geom
./Resources/shaders/OpenGL/visualize_normals.vert
./Sampler/OpenGL/OpenGLSampler.cpp
./Sampler/OpenGL/OpenGLSampler.h
./Sampler/SamplerDescriptor.h
./Sampler/SamplerHandle.h
./SceneGraph/SceneGraph.cpp
./SceneGraph/SceneGraph.h
./SceneGraph/SceneGraphHandle.h
./SceneGraph/SceneNode.cpp
./SceneGraph/SceneNode.h
./SceneGraph/SceneNodeHandle.h
./Shader/Handle/ShaderHandle.h
./Shader/Manager/OpenGL/OpenGLShaderManager.cpp
./Shader/Manager/OpenGL/OpenGLShaderManager.h
./Shader/Module/ShaderModuleDescriptor.h
./Shader/Program/OpenGL/OpenGLShaderProgram.cpp
./Shader/Program/OpenGL/OpenGLShaderProgram.h
./Shader/Program/OpenGL/OpenGLShaderProgramComparator.h
./Shader/Program/ShaderProgramDescriptor.h
./Shader/ShaderStage/OpenGL/OpenGLShaderStage.cpp
./Shader/ShaderStage/OpenGL/OpenGLShaderStage.h
./Shader/ShaderStage/ShaderStage.cpp
./Shader/ShaderStage/ShaderStage.h
./Shader/UniformDataKind.h
./Swapchain/OpenGL/OpenGLSwapchain.cpp
./Swapchain/OpenGL/OpenGLSwapchain.h
./Swapchain/Swapchain.h
./Swapchain/SwapchainHandle.h
./Texture/OpenGL/OpenGLTextureFormat.cpp
./Texture/OpenGL/OpenGLTextureFormat.h
./Texture/Texture.h
./Texture/Texture2DHandle.h
./Texture/TextureDescription.h
./Texture/TextureFormat.cpp
./Texture/TextureFormat.h
./Texture/TextureHandle.h
./Texture/TextureUsage.h
./Texture/TextureView.h
./Texture/TextureViewDescription.h
./Topology/PrimitiveTopology.h
./Transform/Transform.h
./VertexBuffer/OpenGL/OpenGLVertexBuffer.cpp
./VertexBuffer/OpenGL/OpenGLVertexBuffer.h
./VertexBuffer/VertexBuffer.cpp
./VertexBuffer/VertexBuffer.h
./VertexLayout/OpenGL/OpenGLVertexFormat.cpp
./VertexLayout/OpenGL/OpenGLVertexFormat.h
./VertexLayout/OpenGL/OpenGLVertexLayout.h
./VertexLayout/OpenGL/OpenGLVertexLayoutComparator.h
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
