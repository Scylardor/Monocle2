

set(Monocle_Graphics_SOURCES
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
./Camera/ProjectionPlanes.h
./Camera/Viewport.h
./Camera/ViewportDescriptor.h
./Camera/ViewportHandle.h
./Color/Color.cpp
./Color/Color.h
./CommandBuffer/CommandBuffer.cpp
./CommandBuffer/CommandBuffer.h
./Device/GraphicsDevice.h
./Device/OpenGL/OpenGLGraphicsDevice.cpp
./Device/OpenGL/OpenGLGraphicsDevice.h
./Device/Vulkan/VulkanDevice.cpp
./Device/Vulkan/VulkanDevice.h
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
./Drawable/Drawable.cpp
./Drawable/Drawable.h
./Framebuffer/Framebuffer.h
./Framebuffer/FramebufferAttachments.h
./Framebuffer/FramebufferDescription.h
./Framebuffer/FramebufferHandle.h
./Framebuffer/OpenGL/OpenGLFramebuffer.cpp
./Framebuffer/OpenGL/OpenGLFramebuffer.h
./Geometry/Cube.cpp
./Geometry/Cube.h
./GraphicsAllocator/OpenGL/OpenGLBuddyAllocator.cpp
./GraphicsAllocator/OpenGL/OpenGLBuddyAllocator.h
./Handle/DeviceHandles.h
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
./Model/Model.cpp
./Model/Model.h
./OpenGL/moeOpenGL.h
./OpenGL/Std140.h
./Pipeline/OpenGL/OpenGLPipeline.cpp
./Pipeline/OpenGL/OpenGLPipeline.h
./Pipeline/PipelineDescriptor.h
./Pipeline/PipelineHandle.h
./Renderer/AbstractRenderer/AbstractRenderer.cpp
./Renderer/AbstractRenderer/AbstractRenderer.h
./Renderer/OpenGL/OpenGLRenderer.cpp
./Renderer/OpenGL/OpenGLRenderer.h
./Renderer/Renderer.h
./Renderer/RendererDescriptor.h
./Renderer/Vulkan/VulkanRenderer.cpp
./Renderer/Vulkan/VulkanRenderer.h
./RenderQueue/RenderQueue.cpp
./RenderQueue/RenderQueue.h
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
./Resources/shaders/OpenGL/basic_textured.frag
./Resources/shaders/OpenGL/basic_textured.vert
./Resources/shaders/OpenGL/blending.frag
./Resources/shaders/OpenGL/blending.vert
./Resources/shaders/OpenGL/blinn_phong.frag
./Resources/shaders/OpenGL/blinn_phong.vert
./Resources/shaders/OpenGL/blinn_phong_normal_mapping.frag
./Resources/shaders/OpenGL/blinn_phong_normal_mapping.vert
./Resources/shaders/OpenGL/blinn_phong_parallax_mapping.frag
./Resources/shaders/OpenGL/blinn_phong_parallax_mapping.vert
./Resources/shaders/OpenGL/bloom_blinn_phong.frag
./Resources/shaders/OpenGL/bloom_blinn_phong.vert
./Resources/shaders/OpenGL/bloom_blur.frag
./Resources/shaders/OpenGL/bloom_blur.vert
./Resources/shaders/OpenGL/bloom_final.frag
./Resources/shaders/OpenGL/bloom_final.vert
./Resources/shaders/OpenGL/bloom_light_box.frag
./Resources/shaders/OpenGL/brdf_lut.frag
./Resources/shaders/OpenGL/brdf_lut.vert
./Resources/shaders/OpenGL/cubemaps.frag
./Resources/shaders/OpenGL/cubemaps.vert
./Resources/shaders/OpenGL/deferred_gbuffer.frag
./Resources/shaders/OpenGL/deferred_gbuffer.vert
./Resources/shaders/OpenGL/deferred_lighting_pass.frag
./Resources/shaders/OpenGL/deferred_lighting_pass.vert
./Resources/shaders/OpenGL/depth_map.frag
./Resources/shaders/OpenGL/depth_map.vert
./Resources/shaders/OpenGL/depth_map_omnidirectional.frag
./Resources/shaders/OpenGL/depth_map_omnidirectional.geom
./Resources/shaders/OpenGL/depth_map_omnidirectional.vert
./Resources/shaders/OpenGL/depth_testing.frag
./Resources/shaders/OpenGL/depth_testing.vert
./Resources/shaders/OpenGL/explode.frag
./Resources/shaders/OpenGL/explode.geom
./Resources/shaders/OpenGL/explode.vert
./Resources/shaders/OpenGL/framebuffer.frag
./Resources/shaders/OpenGL/framebuffer.vert
./Resources/shaders/OpenGL/fullscreen_quad.frag
./Resources/shaders/OpenGL/fullscreen_quad.vert
./Resources/shaders/OpenGL/fullscreen_quad2.frag
./Resources/shaders/OpenGL/fullscreen_quad2.vert
./Resources/shaders/OpenGL/gamma_correction.frag
./Resources/shaders/OpenGL/gamma_correction.vert
./Resources/shaders/OpenGL/geometry_shader.frag
./Resources/shaders/OpenGL/geometry_shader.geom
./Resources/shaders/OpenGL/geometry_shader.vert
./Resources/shaders/OpenGL/hdr_blinn_phong.frag
./Resources/shaders/OpenGL/hdr_blinn_phong.vert
./Resources/shaders/OpenGL/hdr_blit.frag
./Resources/shaders/OpenGL/hdr_blit.vert
./Resources/shaders/OpenGL/instancing.frag
./Resources/shaders/OpenGL/instancing.vert
./Resources/shaders/OpenGL/light.frag
./Resources/shaders/OpenGL/light.vert
./Resources/shaders/OpenGL/omnidirectional_shadow_mapping.frag
./Resources/shaders/OpenGL/omnidirectional_shadow_mapping.vert
./Resources/shaders/OpenGL/pbr_constant.frag
./Resources/shaders/OpenGL/pbr_constant.vert
./Resources/shaders/OpenGL/pbr_constant_IBL.frag
./Resources/shaders/OpenGL/pbr_cubemap.vert
./Resources/shaders/OpenGL/pbr_equirectangular_to_cubemap.frag
./Resources/shaders/OpenGL/pbr_hdr_background.frag
./Resources/shaders/OpenGL/pbr_hdr_background.vert
./Resources/shaders/OpenGL/pbr_irradiance_convolution.frag
./Resources/shaders/OpenGL/pbr_prefilter_environment.frag
./Resources/shaders/OpenGL/pbr_textured.frag
./Resources/shaders/OpenGL/pbr_textured.vert
./Resources/shaders/OpenGL/pbr_textured_IBL.frag
./Resources/shaders/OpenGL/phong.frag
./Resources/shaders/OpenGL/phong.vert
./Resources/shaders/OpenGL/phong_maps.frag
./Resources/shaders/OpenGL/phong_maps.vert
./Resources/shaders/OpenGL/shadow_mapping.frag
./Resources/shaders/OpenGL/shadow_mapping.vert
./Resources/shaders/OpenGL/skybox.frag
./Resources/shaders/OpenGL/skybox.vert
./Resources/shaders/OpenGL/ssao.frag
./Resources/shaders/OpenGL/ssao.vert
./Resources/shaders/OpenGL/ssao_blur.frag
./Resources/shaders/OpenGL/ssao_blur.vert
./Resources/shaders/OpenGL/ssao_geometry.frag
./Resources/shaders/OpenGL/ssao_geometry.vert
./Resources/shaders/OpenGL/ssao_lighting_pass.frag
./Resources/shaders/OpenGL/ssao_lighting_pass.vert
./Resources/shaders/OpenGL/stencil_testing.frag
./Resources/shaders/OpenGL/stencil_testing.vert
./Resources/shaders/OpenGL/visualize_normals.frag
./Resources/shaders/OpenGL/visualize_normals.geom
./Resources/shaders/OpenGL/visualize_normals.vert
./Resources/shaders/Vulkan/triangle.frag
./Resources/shaders/Vulkan/triangle.vert
./RHI/BufferManager/BufferManager.h
./RHI/FramebufferManager/FramebufferManager.h
./RHI/MaterialManager/MaterialManager.h
./RHI/OpenGL/BufferManager/BufferManager.cpp
./RHI/OpenGL/BufferManager/BufferManager.h
./RHI/OpenGL/FramebufferManager/FramebufferManager.cpp
./RHI/OpenGL/FramebufferManager/FramebufferManager.h
./RHI/OpenGL/MaterialManager/MaterialManager.cpp
./RHI/OpenGL/MaterialManager/MaterialManager.h
./RHI/OpenGL/OGL4RHI.cpp
./RHI/OpenGL/OGL4RHI.h
./RHI/OpenGL/PipelineManager/PipelineManager.cpp
./RHI/OpenGL/PipelineManager/PipelineManager.h
./RHI/OpenGL/RenderPass/OGL4RenderPass.cpp
./RHI/OpenGL/RenderPass/OGL4RenderPass.h
./RHI/OpenGL/SwapchainManager/SwapchainManager.cpp
./RHI/OpenGL/SwapchainManager/SwapchainManager.h
./RHI/OpenGL/TextureManager/TextureManager.cpp
./RHI/OpenGL/TextureManager/TextureManager.h
./RHI/PipelineManager/PipelineManager.h
./RHI/RenderHardwareInterface.cpp
./RHI/RenderHardwareInterface.h
./RHI/SwapchainManager/SwapchainManager.h
./RHI/TextureManager/TextureManager.h
./RHI/Vulkan/VulkanRHI.cpp
./RHI/Vulkan/VulkanRHI.h
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
./Shader/UniformDataKind.h
./ShaderCapabilities/ShaderCapabilities.cpp
./ShaderCapabilities/ShaderCapabilities.h
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
./Transform/Transform.h
./Vertex/VertexFormats.h
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
./Vulkan/Allocators/BufferAllocator.cpp
./Vulkan/Allocators/BufferAllocator.h
./Vulkan/Allocators/MemoryAllocator.cpp
./Vulkan/Allocators/MemoryAllocator.h
./Vulkan/Allocators/TextureAllocator.cpp
./Vulkan/Allocators/TextureAllocator.h
./Vulkan/Buffer/VulkanBuffer.cpp
./Vulkan/Buffer/VulkanBuffer.h
./Vulkan/BufferPoolAllocator/BufferPoolAllocator.cpp
./Vulkan/BufferPoolAllocator/BufferPoolAllocator.h
./Vulkan/Camera/CameraSystem.cpp
./Vulkan/Camera/CameraSystem.h
./Vulkan/CommandPipeline/VulkanCommandPipeline.cpp
./Vulkan/CommandPipeline/VulkanCommandPipeline.h
./Vulkan/CommandPool/VulkanCommandPool.cpp
./Vulkan/CommandPool/VulkanCommandPool.h
./Vulkan/DebugMessenger/VulkanDebugMessenger.cpp
./Vulkan/DebugMessenger/VulkanDebugMessenger.h
./Vulkan/DescriptorSet/VulkanDescriptorSetAllocator.cpp
./Vulkan/DescriptorSet/VulkanDescriptorSetAllocator.h
./Vulkan/Device/VulkanDevice.cpp
./Vulkan/Device/VulkanDevice.h
./Vulkan/Device/VulkanDeviceList.cpp
./Vulkan/Device/VulkanDeviceList.h
./Vulkan/Factories/VulkanMeshFactory.cpp
./Vulkan/Factories/VulkanMeshFactory.h
./Vulkan/Factories/VulkanTextureFactory.cpp
./Vulkan/Factories/VulkanTextureFactory.h
./Vulkan/Framebuffer/FramebufferFactory.cpp
./Vulkan/Framebuffer/FramebufferFactory.h
./Vulkan/Framebuffer/VulkanFramebuffer.cpp
./Vulkan/Framebuffer/VulkanFramebuffer.h
./Vulkan/FrameGraph/VulkanFrameGraph.cpp
./Vulkan/FrameGraph/VulkanFrameGraph.h
./Vulkan/Instance/VulkanInstance.cpp
./Vulkan/Instance/VulkanInstance.h
./Vulkan/Light/VulkanLightSystem.cpp
./Vulkan/Light/VulkanLightSystem.h
./Vulkan/Material/VulkanMaterial.cpp
./Vulkan/Material/VulkanMaterial.h
./Vulkan/Material/VulkanMaterialFactory.cpp
./Vulkan/Material/VulkanMaterialFactory.h
./Vulkan/Mesh/VulkanMesh.cpp
./Vulkan/Mesh/VulkanMesh.h
./Vulkan/Model/VulkanModel.cpp
./Vulkan/Model/VulkanModel.h
./Vulkan/Pipeline/VulkanPipeline.cpp
./Vulkan/Pipeline/VulkanPipeline.h
./Vulkan/RenderPass/VulkanRenderPass.cpp
./Vulkan/RenderPass/VulkanRenderPass.h
./Vulkan/RenderPass/VulkanRenderPassBuilder.cpp
./Vulkan/RenderPass/VulkanRenderPassBuilder.h
./Vulkan/RenderScene/RenderScene.cpp
./Vulkan/RenderScene/RenderScene.h
./Vulkan/Shader/VulkanShaderFactory.cpp
./Vulkan/Shader/VulkanShaderFactory.h
./Vulkan/Shader/VulkanShaderModule.cpp
./Vulkan/Shader/VulkanShaderModule.h
./Vulkan/Shader/VulkanShaderProgram.cpp
./Vulkan/Shader/VulkanShaderProgram.h
./Vulkan/Surface/VulkanSurfaceProvider.h
./Vulkan/Swapchain/VulkanSwapchain.cpp
./Vulkan/Swapchain/VulkanSwapchain.h
./Vulkan/Swapchain/VulkanSwapchainFrame.cpp
./Vulkan/Swapchain/VulkanSwapchainFrame.h
./Vulkan/Swapchain/VulkanSwapchainTexture.cpp
./Vulkan/Swapchain/VulkanSwapchainTexture.h
./Vulkan/Texture/VulkanTexture.cpp
./Vulkan/Texture/VulkanTexture.h
./Vulkan/Texture/VulkanTextureFactory.cpp
./Vulkan/Texture/VulkanTextureFactory.h
./Vulkan/ValidationLayers/VulkanValidationLayers.h
./Vulkan/VulkanMacros.h
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
