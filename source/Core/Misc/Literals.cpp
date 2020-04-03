// Monocle Game Engine source files - Alexandre Baron

#include "Literals.h"

moe::Width_t operator "" _width(unsigned long long val)
{
	return moe::Width_t(uint32_t(val));
}


moe::Height_t operator "" _height(unsigned long long val)
{
	return moe::Height_t(uint32_t(val));
}