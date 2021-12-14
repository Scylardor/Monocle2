// Monocle Game Engine source files - Alexandre Baron

#include "WindowService.h"

namespace moe
{

	void WindowService::Update()
	{
		for (auto& winPtr : m_windows)
		{
			winPtr->PollEvents();
		}

		// Delete all the windows that should get closed.
		// TODO: use the same system as Engine : a registration system instead of a O(n) loop each frame...
		m_windows.Erase(std::remove_if(m_windows.Begin(), m_windows.End(),
			[](auto& windowPtr)
			{
				return windowPtr->ShouldClose();
			}), m_windows.End());
	}
}
