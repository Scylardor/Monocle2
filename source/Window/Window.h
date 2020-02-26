// Monocle Game Engine source files - Alexandre Baron

#pragma once

namespace moe
{
	/* Window interface. All Window types (Glfw, SDL, Win32 etc...) will inherit from that pure virtual class. */
	class IWindow
	{
	public:
		virtual ~IWindow() {}
	};

}