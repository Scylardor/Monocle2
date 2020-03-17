

set(Monocle_Input_SOURCES
	./GLFW/GlfwInputHandler/GlfwInputHandler.cpp
./GLFW/GlfwInputHandler/GlfwInputHandler.h
./Input.h
./InputDescriptor/InputDescriptors.h
./InputEventSink/InputEventsSink.h
./InputHandler/InputHandler.h
./Keyboard/KeyboardMapping.h
./Keyboard/MonocleKeyboardMap.h
./Mouse/MonocleMouse.h
./Private/Input.cpp
./Private/InputEventsSink.cpp
./Private/InputHandler.cpp
./Private/KeyboardMapping.cpp
./RawInput/RawInputHandler.h
./RawInput/RawInputHandlerInterface.h
	)
	
if(WIN32)
	set(Monocle_Input_SOURCES
	
	${Monocle_Input_SOURCES} 
	
	./Mouse/Windows/MonocleMouse_Win32.h
./Private/Windows/MonocleKeyboardMap_Win32.cpp
./Private/Windows/RawInputHandler_Win32.cpp
./RawInput/Windows/RawInputHandler_Win32.h
	)
	
elseif(APPLE)
	set(Monocle_Input_SOURCES
	
	${Monocle_Input_SOURCES} 
	
	
	)
	
elseif(UNIX)
	set(Monocle_Input_SOURCES
	
	${Monocle_Input_SOURCES} 
	
	
	)
	
	
endif()
