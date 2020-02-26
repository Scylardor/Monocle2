// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include <cstdint>

#include <Windows.h>

#include "Core/Misc/moeEnum.h"


/*
	The definitions for all mouse button events on Windows.
*/
namespace moe
{
	// Basing the Enum on uint16_t because Windows uses USHORT.
	MOE_ENUM(MouseButtonEvent, std::uint16_t,
		MOUSE_LEFT_BUTTON_DOWN = RI_MOUSE_LEFT_BUTTON_DOWN,
		MOUSE_LEFT_BUTTON_UP = RI_MOUSE_LEFT_BUTTON_UP,
		MOUSE_MIDDLE_BUTTON_DOWN = RI_MOUSE_MIDDLE_BUTTON_DOWN,
		MOUSE_MIDDLE_BUTTON_UP = RI_MOUSE_MIDDLE_BUTTON_UP,
		MOUSE_RIGHT_BUTTON_DOWN = RI_MOUSE_RIGHT_BUTTON_DOWN,
		MOUSE_RIGHT_BUTTON_UP = RI_MOUSE_RIGHT_BUTTON_UP,
		MOUSE_BUTTON_1_DOWN = RI_MOUSE_BUTTON_1_DOWN,
		MOUSE_BUTTON_1_UP = RI_MOUSE_BUTTON_1_UP,
		MOUSE_BUTTON_2_DOWN = RI_MOUSE_BUTTON_2_DOWN,
		MOUSE_BUTTON_2_UP = RI_MOUSE_BUTTON_2_UP,
		MOUSE_BUTTON_3_DOWN = RI_MOUSE_BUTTON_3_DOWN,
		MOUSE_BUTTON_3_UP = RI_MOUSE_BUTTON_3_UP,
		MOUSE_BUTTON_4_DOWN = RI_MOUSE_BUTTON_4_DOWN,
		MOUSE_BUTTON_4_UP = RI_MOUSE_BUTTON_4_UP,
		MOUSE_BUTTON_5_DOWN = RI_MOUSE_BUTTON_5_DOWN,
		MOUSE_BUTTON_5_UP = RI_MOUSE_BUTTON_5_UP,
		MOUSE_UNKNOWN
	);
}