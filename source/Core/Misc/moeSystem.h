// Monocle Game Engine source files - Alexandre Baron

#pragma once

namespace moe
{
	constexpr bool	IsWindows()
	{
#ifdef _WIN32
		return true;
#else
		return false;
#endif
	}

	constexpr bool	IsLinux()
	{
#ifdef __linux__
		return true;
#else
		return false;
#endif
	}

	constexpr bool	IsMac()
	{
#ifdef __APPLE__
		return true;
#else
		return false;
#endif
	}

}