

set(Monocle_Core_SOURCES
	./Containers/Array/Array.h
./Containers/AssetRegistry/ObjectRegistry.h
./Containers/AssetRegistry/ObjectRegistry.inl
./Containers/FreeList/detail/FreeListObject.h
./Containers/FreeList/Freelist.h
./Containers/FreeList/Freelist.hpp
./Containers/FreeList/PolymorphicFreelist.h
./Containers/FreeList/PolymorphicFreelist.hpp
./Containers/HashMap/HashMap.h
./Containers/IntrusiveListNode.h
./Containers/ObjectPool/ObjectPool.h
./Containers/Private/IntrusiveListNode.internal.hpp
./Containers/SparseArray/SparseArray.h
./Containers/Vector/Vector.h
./Debugger/moeDebugger.h
./Delegates/Delegate.h
./Delegates/event.h
./HashString/HashString.h
./Log/LogUtils.h
./Log/moeLog.h
./Log/moeLogger.h
./Log/moeLoggerBase.h
./Log/moeLogMacros.h
./Log/Policies/CaptureWritePolicy.h
./Log/Policies/DebuggerFormatPolicy.h
./Log/Policies/FileWritePolicy.h
./Log/Policies/IdeWritePolicy.h
./Log/Policies/NoFilterPolicy.h
./Log/Policies/NoFormatPolicy.h
./Log/Policies/NoWritePolicy.h
./Log/Policies/OutStreamWritePolicy.h
./Log/Policies/SeverityFilterPolicy.h
./Log/Private/LogUtils.cpp
./Log/Private/moeLogger.internal.hpp
./Log/Private/moeLoggerBase.cpp
./Log/Private/moeLoggerBase.internal.hpp
./Log/Private/Policies/CaptureWritePolicy.cpp
./Log/Private/Policies/DebuggerFormatPolicy.cpp
./Log/Private/Policies/FileWritePolicy.cpp
./Log/Private/Policies/NoFilterPolicy.cpp
./Log/Private/Policies/NoFormatPolicy.cpp
./Log/Private/Policies/OutStreamWritePolicy.cpp
./Log/Private/Policies/SeverityFilterPolicy.cpp
./Misc/Literals.cpp
./Misc/Literals.h
./Misc/moeAbort.h
./Misc/moeCountof.h
./Misc/moeCRTP.h
./Misc/moeEnum.h
./Misc/moeError.h
./Misc/moeFalse.h
./Misc/moeFile.h
./Misc/moeNamedType.h
./Misc/moeTypeList.h
./Misc/Private/moeAbort.cpp
./Misc/Private/moeFile.cpp
./Misc/Types.h
./Patterns/moeSingleton.h
./Preprocessor/moeAssert.h
./Preprocessor/moeBuilder.h
./Preprocessor/moeDebug.h
./Preprocessor/moeDiscard.h
./Preprocessor/moeJoin.h
./Preprocessor/moeLikely.h
./Preprocessor/moeStringize.h
./Preprocessor/moeUnusedParameter.h
./Preprocessor/Private/moeAssert.cpp
./StringFormat/moeStringFormat.h
./StringFormat/Private/moeStringFormat.internal.hpp
	)
	
if(WIN32)
	set(Monocle_Core_SOURCES
	
	${Monocle_Core_SOURCES} 
	
	./Debugger/Private/Windows/moeDebugger.cpp
./Log/Policies/Windows/Win_IdeWritePolicy.h
./Log/Private/Policies/Windows/Win_IdeWritePolicy.cpp
./Misc/Private/Windows/GetLastErrorAsString.cpp
./Misc/Windows/GetLastErrorAsString.h
./StringFormat/Private/Windows/Win_moeSwprintf.internal.hpp
	)
	
elseif(APPLE)
	set(Monocle_Core_SOURCES
	
	${Monocle_Core_SOURCES} 
	
	
	)
	
elseif(UNIX)
	set(Monocle_Core_SOURCES
	
	${Monocle_Core_SOURCES} 
	
	./Debugger/Private/Linux/moeDebugger.cpp
./Log/Policies/Linux/Linux_IdeWritePolicy.h
./Log/Private/Policies/Linux/Linux_IdeWritePolicy.cpp
./StringFormat/Private/Linux/Linux_moeSwprintf.internal.hpp
	)
	
	
endif()
