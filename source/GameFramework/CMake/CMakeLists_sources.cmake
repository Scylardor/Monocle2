

set(Monocle_GameFramework_SOURCES
	./FSM/FSM.h
./FSM/Private/FSM.cpp
./Resources/Factories/AbstractResourceFactory.h
./Resources/Factories/ResourceFactory.h
./Resources/Factories/Texture/OpenGL/GL3TextureFactory.h
./Resources/Factories/Texture/OpenGL/GL4TextureFactory.cpp
./Resources/Factories/Texture/OpenGL/GL4TextureFactory.h
./Resources/Factories/Texture/OpenGL/GLTextureFactory.h
./Resources/Factories/Texture/TextureFactory.h
./Resources/Resource/GLTextureResource.h
./Resources/Resource/Resource.h
./Resources/Resource/TextureResource.h
./Resources/ResourceManager/ResourceManager.cpp
./Resources/ResourceManager/ResourceManager.h
./Threading/Render/GLFW/GlfwRenderThread.cpp
./Threading/Render/GLFW/GlfwRenderThread.h
./Threading/Render/RenderThread.h
./Threading/ThreadPool/ThreadPool.cpp
./Threading/ThreadPool/ThreadPool.h
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
