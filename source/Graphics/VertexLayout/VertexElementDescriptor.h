// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_STD_SUPPORT
#include <string_view>
#endif

#include "Monocle_Graphics_Export.h"

#include "Graphics/VertexLayout/VertexElementFormat.h"

namespace moe
{

	/**
	 * \brief A graphics API-agnostic descriptor of a vertex element.
	 * It describes a single element part of a vertex layout.
	 * It always comes with a semantic, a.k.a. the "name" of this element, and the data format of this element.
	 */
	struct VertexElementDescriptor
	{
		Monocle_Graphics_API	VertexElementDescriptor(const char * semantic, VertexElementFormat format) :
			m_semantic(semantic), m_format(format)
		{}

		bool operator==(const VertexElementDescriptor& rhs) const
		{
			if (&rhs != this)
			{
				return (m_semantic == rhs.m_semantic && m_format == rhs.m_format);
			}
			return true;
		}

		std::string			m_semantic;
		VertexElementFormat	m_format;
	};

}