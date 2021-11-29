

set(Monocle_GameFramework_SOURCES
	./Engine/Engine.cpp
./Engine/Engine.h
./Engine/Engine.inl
./Engine/EnginePart.h
./FSM/FSM.h
./FSM/Private/FSM.cpp
./Resources/AssetImporter/AssimpAssetImporter.cpp
./Resources/AssetImporter/AssimpAssetImporter.h
./Resources/Factories/AbstractResourceFactory.h
./Resources/Factories/ResourceFactory.h
./Resources/Factories/Texture/OpenGL/GL3TextureFactory.h
./Resources/Factories/Texture/OpenGL/GL4TextureFactory.cpp
./Resources/Factories/Texture/OpenGL/GL4TextureFactory.h
./Resources/Factories/Texture/OpenGL/GLTextureFactory.h
./Resources/Factories/Texture/TextureFactory.h
./Resources/Resource/GLTextureResource.h
./Resources/Resource/TextureResource.h
./Service/ConfigService/ConfigService.cpp
./Service/ConfigService/ConfigService.h
./Service/ConfigService/ConfigService.inl
./Service/LogService/LogService.cpp
./Service/LogService/LogService.h
./Service/LogService/LogService.inl
./Service/RenderService/GraphicsSurface/GraphicsSurface.h
./Service/RenderService/MaterialInstance/MaterialInstance.cpp
./Service/RenderService/MaterialInstance/MaterialInstance.h
./Service/RenderService/MaterialLayout/MaterialLayout.cpp
./Service/RenderService/MaterialLayout/MaterialLayout.h
./Service/RenderService/MaterialModule/MaterialModule.cpp
./Service/RenderService/MaterialModule/MaterialModule.h
./Service/RenderService/MaterialSystem/MaterialSystem.cpp
./Service/RenderService/MaterialSystem/MaterialSystem.h
./Service/RenderService/Renderable/Renderable.cpp
./Service/RenderService/Renderable/Renderable.h
./Service/RenderService/Renderer/Renderer.cpp
./Service/RenderService/Renderer/Renderer.h
./Service/RenderService/RenderPass/GeometryRenderPass.cpp
./Service/RenderService/RenderPass/GeometryRenderPass.h
./Service/RenderService/RenderPass/RenderPass.cpp
./Service/RenderService/RenderPass/RenderPass.h
./Service/RenderService/RenderPass/RenderPassType.h
./Service/RenderService/RenderScene/RenderObject.cpp
./Service/RenderService/RenderScene/RenderObject.h
./Service/RenderService/RenderScene/RenderScene.cpp
./Service/RenderService/RenderScene/RenderScene.h
./Service/RenderService/RenderService.cpp
./Service/RenderService/RenderService.h
./Service/ResourceService/ResourceService.cpp
./Service/ResourceService/ResourceService.h
./Service/Service.h
./Service/TimeService/Durations.h
./Service/TimeService/Stopwatch/Stopwatch.cpp
./Service/TimeService/Stopwatch/Stopwatch.h
./Service/TimeService/Stopwatch/Stopwatch.inl
./Service/TimeService/TimeService.cpp
./Service/TimeService/TimeService.h
./Service/WindowService/GLFWService/GLFWService.cpp
./Service/WindowService/GLFWService/GLFWService.h
./Service/WindowService/GLFWService/OpenGLGLFWService.cpp
./Service/WindowService/GLFWService/OpenGLGLFWService.h
./Service/WindowService/Window/GLFWWindow.cpp
./Service/WindowService/Window/GLFWWindow.h
./Service/WindowService/Window/OpenGLGLFWWindow.cpp
./Service/WindowService/Window/OpenGLGLFWWindow.h
./Service/WindowService/Window/Window.h
./Service/WindowService/WindowService.h
./Service/WindowService/WindowService.inl
./Simulation/App3D/App3D.cpp
./Simulation/App3D/App3D.h
./Simulation/App3D/OpenGLApp3D.cpp
./Simulation/App3D/OpenGLApp3D.h
./Simulation/Simulation.cpp
./Simulation/Simulation.h
./Simulation/Simulation.inl
	)

if(WIN32)
	set(Monocle_GameFramework_SOURCES

	${Monocle_GameFramework_SOURCES}

	
	)

elseif(APPLE)
	set(Monocle_GameFramework_SOURCES

	${Monocle_GameFramework_SOURCES}

	
	)

elseif(UNIX)
	set(Monocle_GameFramework_SOURCES

	${Monocle_GameFramework_SOURCES}

	
	)


endif()
