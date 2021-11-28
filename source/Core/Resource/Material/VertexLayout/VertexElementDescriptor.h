// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_STD_SUPPORT
#include <string_view>
#endif

#include "Monocle_Core_Export.h"

#include "Core/Resource/Material/VertexLayout/VertexElementFormat.h"

namespace moe
{

	/**
	 * \brief A graphics API-agnostic Description of a vertex element.
	 * It describes a single element part of a vertex layout.
	 * It always comes with a semantic, a.k.a. the "name" of this element, and the data format of this element.
	 */
	struct VertexElementDescription
	{
		Monocle_Core_API	VertexElementDescription(const char * semantic, VertexElementFormat format) :
			m_semantic(semantic), m_format(format)
		{}

		bool operator==(const VertexElementDescription& rhs) const
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