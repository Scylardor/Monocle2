// Monocle Game Engine source files - Alexandre Baron

#pragma once

// [[nodiscard]] is a C++17 feature so try not to use it on older compilers.

#ifndef __has_cpp_attribute
	# define __has_cpp_attribute(x) 0
#endif

#if __has_cpp_attribute(nodiscard)
	# define MOE_NODISCARD [[nodiscard]]
#else
	# define MOE_NODISCARD
#endif