

set(Monocle_SOURCES
	source/Application/Application.h
source/Application/GlfwApplication/GlfwApplication.cpp
source/Application/GlfwApplication/GlfwApplication.h
source/Core/Containers/Array/Array.h
source/Core/Containers/HashMap/HashMap.h
source/Core/Containers/IntrusiveListNode.h
source/Core/Containers/Private/IntrusiveListNode.internal.hpp
source/Core/Containers/Vector/Vector.h
source/Core/Debugger/moeDebugger.h
source/Core/Delegates/Delegate.h
source/Core/Delegates/event.h
source/Core/HashString/HashString.h
source/Core/Log/LogUtils.h
source/Core/Log/moeLog.h
source/Core/Log/moeLogger.h
source/Core/Log/moeLoggerBase.h
source/Core/Log/moeLogMacros.h
source/Core/Log/Policies/CaptureWritePolicy.h
source/Core/Log/Policies/DebuggerFormatPolicy.h
source/Core/Log/Policies/FileWritePolicy.h
source/Core/Log/Policies/IdeWritePolicy.h
source/Core/Log/Policies/NoFilterPolicy.h
source/Core/Log/Policies/NoFormatPolicy.h
source/Core/Log/Policies/NoWritePolicy.h
source/Core/Log/Policies/OutStreamWritePolicy.h
source/Core/Log/Policies/SeverityFilterPolicy.h
source/Core/Log/Private/LogUtils.cpp
source/Core/Log/Private/moeLogger.internal.hpp
source/Core/Log/Private/moeLoggerBase.cpp
source/Core/Log/Private/moeLoggerBase.internal.hpp
source/Core/Log/Private/Policies/CaptureWritePolicy.cpp
source/Core/Log/Private/Policies/DebuggerFormatPolicy.cpp
source/Core/Log/Private/Policies/FileWritePolicy.cpp
source/Core/Log/Private/Policies/NoFilterPolicy.cpp
source/Core/Log/Private/Policies/NoFormatPolicy.cpp
source/Core/Log/Private/Policies/OutStreamWritePolicy.cpp
source/Core/Log/Private/Policies/SeverityFilterPolicy.cpp
source/Core/Misc/moeAbort.h
source/Core/Misc/moeCountof.h
source/Core/Misc/moeEnum.h
source/Core/Misc/moeError.h
source/Core/Misc/moeFalse.h
source/Core/Misc/moeNamedType.h
source/Core/Misc/moeTypeList.h
source/Core/Misc/Private/moeAbort.cpp
source/Core/Misc/Types.h
source/Core/Patterns/moeSingleton.h
source/Core/Preprocessor/moeAssert.h
source/Core/Preprocessor/moeDLLVisibility.h
source/Core/Preprocessor/moeJoin.h
source/Core/Preprocessor/moeLikely.h
source/Core/Preprocessor/moeStringize.h
source/Core/Preprocessor/moeUnusedParameter.h
source/Core/Preprocessor/Private/moeAssert.cpp
source/Core/StringFormat/moeStringFormat.h
source/Core/StringFormat/Private/moeStringFormat.internal.hpp
source/GameFramework/FSM/FSM.h
source/GameFramework/FSM/Private/FSM.cpp
source/Input/Input.h
source/Input/InputDescriptors.h
source/Input/InputEventsSink.h
source/Input/InputHandler.h
source/Input/KeyboardMapping.h
source/Input/MonocleKeyboardMap.h
source/Input/MonocleMouse.h
source/Input/Private/Input.cpp
source/Input/Private/InputEventsSink.cpp
source/Input/Private/InputHandler.cpp
source/Input/Private/KeyboardMapping.cpp
source/Input/RawInputHandler.h
source/Input/RawInputHandlerInterface.h
source/pch.h
source/Window/Window.h
source/Window/WindowDescriptor.cpp
source/Window/WindowDescriptor.h
	)
	
if(WIN32)
	set(Monocle_SOURCES
	
	${Monocle_SOURCES} 
	
	source/Core/Debugger/Private/Windows/moeDebugger.cpp
source/Core/Log/Policies/Windows/Win_IdeWritePolicy.h
source/Core/Log/Private/Policies/Windows/Win_IdeWritePolicy.cpp
source/Core/Misc/Private/Windows/GetLastErrorAsString.cpp
source/Core/Misc/Windows/GetLastErrorAsString.h
source/Core/StringFormat/Private/Windows/Win_moeSwprintf.internal.hpp
source/Input/Private/Windows/MonocleKeyboardMap_Win32.cpp
source/Input/Private/Windows/RawInputHandler_Win32.cpp
source/Input/Windows/MonocleMouse_Win32.h
source/Input/Windows/RawInputHandler_Win32.h
	)
	
elseif(APPLE)
	set(Monocle_SOURCES
	
	${Monocle_SOURCES} 
	
	
	)
	
elseif(UNIX)
	set(Monocle_SOURCES
	
	${Monocle_SOURCES} 
	
	source/Core/Debugger/Private/Linux/moeDebugger.cpp
source/Core/Log/Policies/Linux/Linux_IdeWritePolicy.h
source/Core/Log/Private/Policies/Linux/Linux_IdeWritePolicy.cpp
source/Core/StringFormat/Private/Linux/Linux_moeSwprintf.internal.hpp
	)
	
	
endif()
