// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/Types.h"
#include "Core/Misc/moeNamedType.h"

namespace moe
{
	struct WindowDescriptor
	{
		struct WidthParam {};
		using Width_t = NamedType<uint32_t, WidthParam>;

		struct HeightParam {};
		using Height_t = NamedType<uint32_t, HeightParam>;

		WindowDescriptor(Width_t w, Height_t h, const wchar_t* windowTitle, const char* iconPath);

		Width_t				m_width;
		Height_t			m_height;
		const wchar_t*      m_title = nullptr;
		const char*			m_iconPath = nullptr;

	};

}