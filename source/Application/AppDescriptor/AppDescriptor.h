// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Literals.h" // width and height

#include "Monocle_Application_Export.h"

namespace moe
{

	/**
	 * \brief AppDescriptor
	 * A small structure containing all you need to specify how to create your application
	 */
	struct  AppDescriptor
	{
		AppDescriptor() = default;

		Monocle_Application_API AppDescriptor(Width_t w, Height_t h, const char* windowTitle, const char* iconPath, bool resizableWin);

		Width_t				m_windowWidth{0};
		Height_t			m_windowHeight{0};
		const char*			m_windowTitle = nullptr;
		const char*			m_windowIcon = nullptr;
		bool				m_resizableWindow = false;

	};

}