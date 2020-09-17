

set(Monocle_GameFramework_SOURCES
	./FSM/FSM.h
./FSM/Private/FSM.cpp
./Resources/Resource/GLTextureResource.h
./Resources/Resource/Resource.h
./Resources/Resource/TextureResource.h
./Resources/ResourceFactory/AbstractResourceFactory.h
./Resources/ResourceFactory/ResourceFactory.h
./Resources/ResourceFactory/TextureFactory.h
./Resources/ResourceManager/ResourceManager.cpp
./Resources/ResourceManager/ResourceManager.h
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
