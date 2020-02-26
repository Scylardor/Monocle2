// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Preprocessor/moeDLLVisibility.h"
#include "Input/InputDescriptors.h"

/*
	The InputEventSink is a holder class meant to store incoming input events.
*/

namespace moe
{

	class MOE_DLL_API InputEventSink
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