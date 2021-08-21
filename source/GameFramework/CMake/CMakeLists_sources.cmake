

set(Monocle_GameFramework_SOURCES
	./Engine/Engine.cpp
./Engine/Engine.h
./Engine/Engine.inl
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
./Simulation/App3D/App3D.cpp
./Simulation/App3D/App3D.h
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
