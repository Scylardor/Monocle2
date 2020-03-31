// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "moeNamedType.h"
#include "Types.h"

#include "Core/Preprocessor/moeDLLVisibility.h"

namespace moe
{
	struct WidthParam {};
	using Width_t = NamedType<uint32_t, WidthParam>;

	struct HeightParam {};
	using Height_t = NamedType<uint32_t, HeightParam>;

}

moe::Width_t MOE_DLL_API operator "" _width(unsigned long long val);

moe::Height_t MOE_DLL_API operator "" _height(unsigned long long val);