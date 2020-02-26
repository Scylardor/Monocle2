// Monocle Game Engine source files - Alexandre Baron

#include "WindowDescriptor.h"


moe::WindowDescriptor::WindowDescriptor(Width_t w, Height_t h, const char * windowTitle, const char * iconPath) :
	m_width(w), m_height(h), m_title(windowTitle), m_iconPath(iconPath)
{

}
