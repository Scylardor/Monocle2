// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include <cstdint>

#include "Core/Misc/moeEnum.h"

namespace moe
{
	typedef	std::uint64_t	InputMappingID;


	/*
	A logical device ID is an ID given on all devices upon their detection
	that allows to identify them from a game code point of view.
	The "default" IDs are special IDs usually attributed to the first device
	detected of each kind, because some systems like Windows declare a kind of
	"virtual super-device" first that matches input from all the devices of this kind,
	which is handy for the usual case of having only one device of each type on the user machine.
	*/
	MOE_ENUM(LogicalDeviceID, int,
		Keyboard_0,
		Keyboard_1,
		Keyboard_2,
		Keyboard_3,
		Keyboard_4,
		Keyboard_5,
		Keyboard_6,
		Keyboard_7,
		Keyboard_MAX,
		Mouse_0,
		Mouse_1,
		Mouse_2,
		Mouse_3,
		Mouse_4,
		Mouse_5,
		Mouse_6,
		Mouse_7,
		Mouse_MAX
	);
	DECLARE_MOE_ENUM_OPERATORS(LogicalDeviceID);

	/* An enum describing the state of a generic "button", it can be a mouse button, keyboard key, gamepad button... */
	MOE_ENUM(ButtonState, std::uint8_t,
		Pressed = 0,
		Repeated,
		Released,
		Unknown
	);
	DECLARE_MOE_ENUM_OPERATORS(ButtonState);


	MOE_ENUM(MouseButtonState, std::uint8_t,
		Down = 0,
		Up,
		Unknown
	);


}