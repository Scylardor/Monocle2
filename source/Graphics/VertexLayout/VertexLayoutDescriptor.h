// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Containers/Vector/Vector.h"
#include "Core/Containers/Array/Array.h"

#include "Graphics/VertexLayout/VertexElementDescriptor.h"


namespace moe
{

	/**
	 * \brief A graphics API-agnostic descriptor of a Vertex Layout.
	 * Vertex layout describes how is the structure of a vertex laid out in memory.
	 * It describes the type of each element, the number of components, whether these are supposed to be normalized or not.
	 * The graphics engine manages data differently whether the data comes in an Interleaved or Packed way.
	 * Usually, Interleaved is a bit more optimized and probably what you want by default.
	 */
	class VertexLayoutDescriptor
	{
	public:
		using VertexElementVector = Vector<VertexElementDescriptor>;
		template <std::size_t N> using VertexElementArray = Array<VertexElementDescriptor, N>;


		/**
		 * \brief Describes whether a layout's elements are supposed to be interleaved (XYZ XYZ XYZ) or packed together (XXX YYY ZZZ).
		 */
		enum LayoutType : unsigned char
		{
			Interleaved = 0,
			Packed
		};


		VertexLayoutDescriptor(const VertexElementVector& vtxElems, LayoutType type = LayoutType::Interleaved) :
			m_vtxElemFmts(vtxElems), m_type(type)
		{}

		VertexLayoutDescriptor(VertexElementVector&& vtxElems, LayoutType type = LayoutType::Interleaved) :
			m_vtxElemFmts(std::move(vtxElems)), m_type(type)
		{}

		template <std::size_t N>
		VertexLayoutDescriptor(VertexElementArray<N>&& vtxArray, LayoutType type = LayoutType::Interleaved) :
			m_vtxElemFmts(vtxArray.Begin(), vtxArray.End()), m_type(type)
		{}


		VertexLayoutDescriptor(std::initializer_list<VertexElementDescriptor> il) :
			m_vtxElemFmts(il) {}


		bool	operator==(const VertexLayoutDescriptor& rhs) const
		{
			if (&rhs != this)
			{
				return (m_vtxElemFmts == rhs.m_vtxElemFmts && m_type == rhs.m_type);
			}
			return true;
		}

		//C++11 range for interface implementation

		VertexElementVector::Iterator	begin()
		{
			return m_vtxElemFmts.Begin();
		}


		VertexElementVector::ConstIterator	begin() const
		{
			return m_vtxElemFmts.Begin();
		}


		VertexElementVector::Iterator	end()
		{
			return m_vtxElemFmts.End();
		}

		VertexElementVector::ConstIterator	end() const
		{
			return m_vtxElemFmts.End();
		}

		LayoutType	Type() const
		{
			return m_type;
		}


		void	SetType(LayoutType type)
		{
			m_type = type;
		}


	private:
		VertexElementVector	m_vtxElemFmts;
		LayoutType	m_type{LayoutType::Interleaved};
	};

}