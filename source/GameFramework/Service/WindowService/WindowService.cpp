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
	}
}
