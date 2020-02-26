// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/moeEnum.h"

/*
	All low-level utilities to work with the system mouse.
*/

namespace moe
{
	MOE_ENUM(MouseAxis, std::uint8_t,
		MOUSE_X_AXIS,
		MOUSE_Y_AXIS,
		MOUSE_WHEEL_AXIS,
		MOUSE_UNKNOWN_AXIS
	);
}


#if defined(MOE_LINUX)
// ...
#elif defined(MOE_WINDOWS)
#include "Input/Windows/MonocleMouse_Win32.h"
#endif