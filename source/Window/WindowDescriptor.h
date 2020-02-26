// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"
#include "Core/Misc/Literals.h" // width and height

namespace moe
{
	struct WindowDescriptor
	{

		WindowDescriptor(Width_t w, Height_t h, const char* windowTitle = "Monocle Application", const char* iconPath = nullptr);

		Width_t				m_width;
		Height_t			m_height;
		const char*			m_title = nullptr;
		const char*			m_iconPath = nullptr;

	};

}