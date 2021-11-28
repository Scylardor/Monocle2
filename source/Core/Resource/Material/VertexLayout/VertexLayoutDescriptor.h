// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Containers/Vector/Vector.h"
#include "Core/Containers/Array/Array.h"

#include "Core/Resource/Material/VertexLayout/VertexElementDescriptor.h"


namespace moe
{
	/**
	 * \brief Describes whether a layout's elements are supposed to be interleaved (XYZ XYZ XYZ) or packed together (XXX YYY ZZZ).
	 */
	enum class LayoutType : unsigned char
	{
		Interleaved = 0,
		Packed
	};

	using VertexElementVector = Vector<VertexElementDescription>;

	class VertexLayoutBindings
	{
	public:
		VertexLayoutBindings(const VertexElementVector& vec, LayoutType type = LayoutType::Interleaved, uint32_t buffBinding = 0, uint32_t instanceStepRate = 0) :
		m_vtxElemFmts(vec), m_type(type), m_bufferBinding(buffBinding), m_instanceStepRate(instanceStepRate)
		{}

		VertexLayoutBindings(VertexElementVector&& vec, LayoutType type = LayoutType::Interleaved, uint32_t buffBinding = 0, uint32_t instanceStepRate = 0)
		: m_vtxElemFmts(std::move(vec)), m_type(type), m_bufferBinding(buffBinding), m_instanceStepRate(instanceStepRate)
		{}

		bool	operator==(const VertexLayoutBindings& rhs) const
		{
			if (&rhs != this)
			{
				return (m_vtxElemFmts == rhs.m_vtxElemFmts && m_type == rhs.m_type
				&& m_bufferBinding == rhs.m_bufferBinding && m_instanceStepRate == rhs.m_instanceStepRate);
			}
			return true;
		}

		LayoutType	GetLayoutType() const { return m_type; }
		bool		IsInterleaved() const { return m_type == LayoutType::Interleaved; }

		uint32_t	BufferBinding() const { return m_bufferBinding; }

		uint32_t	InstanceStepRate() const { return m_instanceStepRate; }

		void		SetTotalStride(uint32_t stride) { m_totalStride = stride; }
		uint32_t	GetTotalStride() const { return m_totalStride; }


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


	private:
		VertexElementVector	m_vtxElemFmts;
		LayoutType			m_type{ LayoutType::Interleaved };
		uint32_t			m_bufferBinding{0};
		uint32_t			m_instanceStepRate{ 0 };
		uint32_t			m_totalStride{0}; // set by the graphics library
	};

	using VertexBindingsVector = Vector<VertexLayoutBindings>;

	/**
	 * \brief A graphics API-agnostic Description of a Vertex Layout.
	 * Vertex layout describes how is the structure of a vertex laid out in memory.
	 * It describes the type of each element, the number of components, whether these are supposed to be normalized or not.
	 * The graphics engine manages data differently whether the data comes in an Interleaved or Packed way.
	 * Usually, Interleaved is a bit more optimized and probably what you want by default.
	 */
	class VertexLayoutDescription
	{
	public:

		template <std::size_t N> using VertexElementArray = Array<VertexElementDescription, N>;


		VertexLayoutDescription(const VertexElementVector& vtxElems, LayoutType type = LayoutType::Interleaved) :
			m_vtxElemFmts(vtxElems), m_type(type)
		{}

		VertexLayoutDescription(VertexElementVector&& vtxElems, LayoutType type = LayoutType::Interleaved) :
			m_vtxElemFmts(std::move(vtxElems)), m_type(type)
		{}

		template <std::size_t N>
		VertexLayoutDescription(VertexElementArray<N>&& vtxArray, LayoutType type = LayoutType::Interleaved) :
			m_vtxElemFmts(vtxArray.Begin(), vtxArray.End()), m_type(type)
		{}


		VertexLayoutDescription(std::initializer_list<VertexElementDescription> il) :
			m_vtxElemFmts(il) {}


		bool	operator==(const VertexLayoutDescription& rhs) const
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

	/**
	* \brief A graphics API-agnostic Description of a Vertex Layout made to be used with Instancing.
	* Vertex layout describes how is the structure of a vertex laid out in memory.
	* It describes the type of each element, the number of components, whether these are supposed to be normalized or not.
	* The graphics engine manages data differently whether the data comes in an Interleaved or Packed way.
	* Usually, Interleaved is a bit more optimized and probably what you want by default.
	* TODO: This is actually how all vertex layouts should work but was not made to not break any retro-compatibility. Make that the one and only VertexLayoutDescription.
	*/
	class InstancedVertexLayoutDescription
	{
	public:

		template <std::size_t N> using VertexBindingsArray = Array<VertexLayoutBindings, N>;

		InstancedVertexLayoutDescription(const VertexBindingsVector& vtxBindings) :
			m_vtxBindings(vtxBindings)
		{}

		InstancedVertexLayoutDescription(VertexBindingsVector&& vtxBindings) :
			m_vtxBindings(std::move(vtxBindings))
		{}

		template <std::size_t N>
		InstancedVertexLayoutDescription(VertexBindingsArray<N>&& vtxBindings) :
			m_vtxBindings(vtxBindings.Begin(), vtxBindings.End())
		{}


		InstancedVertexLayoutDescription(std::initializer_list<VertexLayoutBindings> il) :
			m_vtxBindings(il) {}


		size_t	NumBindings() const { return m_vtxBindings.Size(); }


		bool	operator==(const InstancedVertexLayoutDescription& rhs) const
		{
			if (&rhs != this)
			{
				return (m_vtxBindings == rhs.m_vtxBindings);
			}
			return true;
		}

		//C++11 range for interface implementation

		VertexBindingsVector::Iterator	begin()
		{
			return m_vtxBindings.Begin();
		}


		VertexBindingsVector::ConstIterator	begin() const
		{
			return m_vtxBindings.Begin();
		}


		VertexBindingsVector::Iterator	end()
		{
			return m_vtxBindings.End();
		}

		VertexBindingsVector::ConstIterator	end() const
		{
			return m_vtxBindings.End();
		}


	private:
		VertexBindingsVector	m_vtxBindings;
	};
}