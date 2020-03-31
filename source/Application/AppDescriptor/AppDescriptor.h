// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Literals.h" // width and height

#include "Core/Preprocessor/moeDLLVisibility.h"

namespace moe
{

	/**
	 * \brief AppDescriptor
	 * A small structure containing all you need to specify how to create your application
	 */
	struct MOE_DLL_API AppDescriptor
	{

		AppDescriptor(Width_t w, Height_t h, const char* windowTitle = "Monocle Application", const char* iconPath = nullptr, bool resizableWin = false);

		Width_t				m_windowWidth;
		Height_t			m_windowHeight;
		const char*			m_windowTitle = nullptr;
		const char*			m_windowIcon = nullptr;
		bool				m_resizableWindow = false;

	};

}