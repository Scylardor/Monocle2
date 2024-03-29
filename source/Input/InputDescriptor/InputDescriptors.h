// Monocle Game Engine source files - Alexandre Baron

#pragma once

// TODO Fix compilation for now because missing raw input handler, but this has to be redone properly.
#if defined(MOE_WINDOWS) && defined(MOE_USE_WIN32)

#include "Input/RawInput/RawInputHandler.h"
#include "Input/Keyboard/MonocleKeyboardMap.h"
#include "Input/Mouse/MonocleMouse.h"

#include <cstdint>


/*
	Input Descriptors that can be used by anyone dealing with Input.
*/
namespace moe
{

	/*
	Describes a keyboard event:
	- a keycode
	- whether it's meant to be fired on key pressed, repeated, or released
	*/
	struct Monocle_Input_API KeyboardEventDesc
	{
		KeyboardEventDesc() = default;

		KeyboardEventDesc(Keycode code, ButtonState state) :
			m_code(code), m_state(state)
		{}

		Keycode		m_code = 0;
		ButtonState	m_state = ButtonState::Unknown;
	};


	struct Monocle_Input_API MouseButtonEventDesc
	{
		MouseButtonEventDesc() = default;

		MouseButtonEventDesc(MouseButtonEvent btnEvent) :
			m_btnEvent(btnEvent)
		{}

		MouseButtonEvent	m_btnEvent = MouseButtonEvent::MOUSE_UNKNOWN;
	};


	struct Monocle_Input_API MouseAxisEventDesc
	{
		MouseAxisEventDesc() = default;

		MouseAxisEventDesc(MouseAxis eventAxis, std::int32_t axisDelta) :
			m_axis(eventAxis), m_axisDelta(axisDelta)
		{}

		MouseAxis		m_axis = MouseAxis::MOUSE_UNKNOWN_AXIS;
		std::int32_t	m_axisDelta = 0;
	};


	struct Monocle_Input_API DeviceArrivalDesc
	{
		DeviceArrivalDesc() = default;

		DeviceArrivalDesc(RawInputHandler::RawInputDeviceID handle, RawInputHandler::ERawInputDeviceTypeID kind) :
			m_deviceHandle(handle), m_deviceKind(kind)
		{}

		RawInputHandler::RawInputDeviceID		m_deviceHandle;
		RawInputHandler::ERawInputDeviceTypeID	m_deviceKind = RawInputHandler::ERawInputDeviceTypeID::DeviceType_Unknown;
	};


	struct Monocle_Input_API DeviceRemovalDesc
	{
		DeviceRemovalDesc() = default;

		DeviceRemovalDesc(RawInputHandler::RawInputDeviceID handle) :
			m_deviceHandle(handle)
		{}

		RawInputHandler::RawInputDeviceID	m_deviceHandle;
	};

}

#endif // defined(MOE_WINDOWS) && defined(MOE_USE_WIN32)