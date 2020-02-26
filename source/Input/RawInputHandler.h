// Monocle Game Engine source files - Alexandre Baron

#pragma once

// By default, define the RawInputHandler type to the correct type depending on the target OS.
#if defined(MOE_LINUX)

// ...

#elif defined(MOE_WINDOWS)

#include "Windows/RawInputHandler_Win32.h"
namespace moe
{
	typedef IRawInputHandler<Win32RawInputHandler>	RawInputHandler;
}

#endif


