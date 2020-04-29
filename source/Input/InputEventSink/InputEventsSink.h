// Monocle Game Engine source files - Alexandre Baron

#pragma once

// TODO Fix compilation for now because missing raw input handler, but this has to be redone properly.
#if defined(MOE_WINDOWS) && defined(MOE_USE_WIN32)

#include "Monocle_Input_Export.h"
#include "Input/InputDescriptor/InputDescriptors.h"
#include "Input/Keyboard/MonocleKeyboardMap.h"


/*
	The InputEventSink is a holder class meant to store incoming input events.
*/

namespace moe
{

	class Monocle_Input_API InputEventSink
	{
	public:

		void	Flush();


		void	EmplaceKeyboardEvent(Keycode code, ButtonState state)
		{
			m_keyboardEvents.EmplaceBack(code, state);
		}


		void	EmplaceMouseButtonEvent(MouseButtonEvent btnEvent)
		{
			m_mouseBtnEvents.EmplaceBack(btnEvent);
		}


		void	EmplaceMouseAxisEvent(MouseAxis eventAxis, std::int32_t axisDelta)
		{
			m_mouseAxisEvents.EmplaceBack(eventAxis, axisDelta);
		}


		const Vector<KeyboardEventDesc>&	GetKeyboardEvents() const
		{
			return m_keyboardEvents;
		}


		const Vector<MouseButtonEventDesc>&	GetMouseButtonEvents() const
		{
			return m_mouseBtnEvents;
		}


		const Vector<MouseAxisEventDesc>&	GetMouseAxisEvents() const
		{
			return m_mouseAxisEvents;
		}


	private:
		Vector<KeyboardEventDesc>		m_keyboardEvents;
		Vector<MouseButtonEventDesc>	m_mouseBtnEvents;
		Vector<MouseAxisEventDesc>		m_mouseAxisEvents;

	};


}

#endif // #if defined(MOE_WINDOWS) && defined(MOE_USE_WIN32)