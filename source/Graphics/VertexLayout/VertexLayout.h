// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "VertexLayoutDescriptor.h"

namespace moe
{

	/**
	 * \brief An API agnostic vertex layout class.
	 * Its primary role is to hold vertex layout description, and it can be extended by specific implementations
	 * to add all the necessary information they need.
	 */
	class VertexLayout
	{

	public:
		VertexLayout(const VertexLayoutDescriptor& layoutDesc) :
			m_desc(layoutDesc)
		{}

		~VertexLayout() = default;

		const	VertexLayoutDescriptor&	ReadDescriptor() const
		{
			return m_desc;
		}

		bool	IsInterleaved() const
		{
			return m_desc.Type() == VertexLayoutDescriptor::Interleaved;
		}


	private:
		VertexLayoutDescriptor	m_desc;
		uint32_t	m_totalBytesSize = 0;
	};

}