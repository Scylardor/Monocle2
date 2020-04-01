// Monocle Game Engine source files - Alexandre Baron

// TODO Fix compilation for now because missing raw input handler, but this has to be redone properly.
#if defined(MOE_WINDOWS) && defined(MOE_USE_WIN32)
#include "Input/InputEventSink/InputEventsSink.h"

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
#endif // defined(MOE_WINDOWS) && defined(MOE_USE_WIN32)