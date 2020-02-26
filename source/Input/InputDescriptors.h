// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Input/RawInputHandler.h"
#include "Input/MonocleKeyboardMap.h"
#include "Input/MonocleMouse.h"

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
	struct MOE_DLL_API KeyboardEventDesc
	{
		KeyboardEventDesc() = default;

		KeyboardEventDesc(Keycode code, ButtonState state) :
			m_code(code), m_state(state)
		{}

		Keycode		m_code = 0;
		ButtonState	m_state = ButtonState::Unknown;
	};


	struct MOE_DLL_API MouseButtonEventDesc
	{
		MouseButtonEventDesc() = default;

		MouseButtonEventDesc(MouseButtonEvent btnEvent) :
			m_btnEvent(btnEvent)
		{}

		MouseButtonEvent	m_btnEvent = MouseButtonEvent::MOUSE_UNKNOWN;
	};


	struct MOE_DLL_API MouseAxisEventDesc
	{
		MouseAxisEventDesc() = default;

		MouseAxisEventDesc(MouseAxis eventAxis, std::int32_t axisDelta) :
			m_axis(eventAxis), m_axisDelta(axisDelta)
		{}

		MouseAxis		m_axis = MouseAxis::MOUSE_UNKNOWN_AXIS;
		std::int32_t	m_axisDelta = 0;
	};


	struct MOE_DLL_API DeviceArrivalDesc
	{
		DeviceArrivalDesc() = default;

		DeviceArrivalDesc(RawInputHandler::RawInputDeviceID handle, RawInputHandler::ERawInputDeviceTypeID kind) :
			m_deviceHandle(handle), m_deviceKind(kind)
		{}

		RawInputHandler::RawInputDeviceID		m_deviceHandle;
		RawInputHandler::ERawInputDeviceTypeID	m_deviceKind = RawInputHandler::ERawInputDeviceTypeID::DeviceType_Unknown;
	};


	struct MOE_DLL_API DeviceRemovalDesc
	{
		DeviceRemovalDesc() = default;

		DeviceRemovalDesc(RawInputHandler::RawInputDeviceID handle) :
			m_deviceHandle(handle)
		{}

		RawInputHandler::RawInputDeviceID	m_deviceHandle;
	};

}