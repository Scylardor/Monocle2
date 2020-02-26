// Monocle Game Engine source files - Alexandre Baron


#include "Input/InputEventsSink.h"

namespace moe
{
	void InputEventSink::Flush()
	{
		// Use Resize(0) instead of Clear, this way we don't lose preallocated memory.
		m_keyboardEvents.Resize(0);
		m_mouseBtnEvents.Resize(0);
		m_mouseAxisEvents.Resize(0);
	}

}
