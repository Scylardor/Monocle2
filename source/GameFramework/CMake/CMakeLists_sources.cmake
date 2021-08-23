

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
./Service/Service.h
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
