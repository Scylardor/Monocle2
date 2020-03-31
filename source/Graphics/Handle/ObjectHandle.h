// Monocle Game Engine source files - Alexandre Baron

#pragma once

namespace moe
{
	/**
	 * \brief A handle that a Renderer can use to uniquely identify a graphics object.
	 * Because graphics APIs have vastly different ways to handle objects, using an opaque handle in our public interfaces
	 * is the easiest option we have (instead of pointers to virtual interfaces, for example).
	 * Each renderer is then free to reinterpret the handle to retrieve the actual data
	 */
	template <typename ValT>
	struct RenderObjectHandle
	{
		ValT	m_handle{ 0 };

		static RenderObjectHandle	Null() { return RenderObjectHandle{0}; }
	};

}
