

set(Monocle_GameFramework_SOURCES
	./FSM/FSM.h
./FSM/Private/FSM.cpp
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
