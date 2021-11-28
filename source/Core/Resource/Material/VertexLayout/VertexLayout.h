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

		VertexLayout(InstancedVertexLayoutDescriptor&& desc) :
			m_instancedDesc(std::move(desc))
		{}


		~VertexLayout() = default;

		const VertexLayoutDescriptor&	ReadDescriptor() const
		{
			return m_desc;
		}

		// TODO: code smell, to refactor
		const InstancedVertexLayoutDescriptor&	ReadInstancedDescriptor() const
		{
			return m_instancedDesc;
		}

		bool	IsInterleaved() const
		{
			return m_desc.Type() == LayoutType::Interleaved;
		}


	private:
		VertexLayoutDescriptor	m_desc{};
		InstancedVertexLayoutDescriptor	m_instancedDesc{}; // TODO: code smell, to remove
		uint32_t	m_totalBytesSize = 0;
	};

}