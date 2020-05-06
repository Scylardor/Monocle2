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
		typedef ValT	Underlying;


		ValT	m_handle{ 0 };

		[[nodiscard]] ValT	Get() const { return m_handle; }

		[[nodiscard]] bool	IsNull() const { return m_handle == 0; }
		[[nodiscard]] bool	IsNotNull() const { return !IsNull(); }

		bool operator==(const RenderObjectHandle& rhs) const
		{
			return m_handle == rhs.m_handle;
		}

		bool operator!=(const RenderObjectHandle& rhs) const
		{
			return !(*this == rhs);
		}

		static RenderObjectHandle	Null() { return RenderObjectHandle{0}; }
	};

}
