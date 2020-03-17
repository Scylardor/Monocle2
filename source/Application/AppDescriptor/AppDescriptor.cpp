// Monocle Game Engine source files - Alexandre Baron

#include "AppDescriptor.h"


moe::AppDescriptor::AppDescriptor(Width_t w, Height_t h, const char * windowTitle, const char * iconPath) :
	m_windowWidth(w), m_windowHeight(h), m_windowTitle(windowTitle), m_windowIcon(iconPath)
{

}
