// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Preprocessor/moeDLLVisibility.h"
#include "Core/Preprocessor/moeAssert.h"

#include <array>

namespace moe
{
	// Implementation of an array
	// Currently relying on the standard library.
	template <class Data, std::size_t N >
	class Array
	{
	public:

		typedef	typename std::array<Data, N>::iterator			Iterator;
		typedef	typename std::array<Data, N>::const_iterator	ConstIterator;
		typedef	typename std::array<Data, N>::value_type		ValueType;
		typedef	typename std::array<Data, N>::size_type			SizeType;

		MOE_DLL_API Array() = default;
		MOE_DLL_API ~Array() = default;

		Array(std::initializer_list<Data> il)
		{
			for (auto elemIt = il.begin(); elemIt != il.end(); ++elemIt)
			{
				m_array[elemIt - il.begin()] = *elemIt;
			}
		}

		size_t	Size() const { return m_array.size(); }

		void	Fill(const ValueType& val) { m_array.fill(val); }

		Iterator		Begin() noexcept		{ return m_array.begin(); }
		ConstIterator	Begin() const noexcept	{ return m_array.begin(); }

		Iterator		End() noexcept			{ return m_array.end(); }
		ConstIterator	End() const noexcept	{ return m_array.end(); }


		Data&		operator[](SizeType idx)
		{
			MOE_DEBUG_ASSERT(idx < N);
			return m_array[idx];
		}

		const Data&	operator[](SizeType idx) const
		{
			MOE_DEBUG_ASSERT(idx < N);
			return m_array[idx];
		}


		// C++11 range-based for loops interface
		Iterator		begin() { return Begin(); }
		ConstIterator	begin() const { return Begin(); }

		Iterator		end() { return End(); }
		ConstIterator	end() const { return End(); }
		//--------------------------------


	private:
		std::array<Data, N>	m_array;


	public:

		// Relational operators soup
		bool operator== (const Array& rhs) const
		{
			return this->m_array == rhs.m_array;
		}

		bool operator!= (const Array& rhs) const
		{
			return this->m_array != rhs.m_array;
		}

		bool operator<  (const Array& rhs) const
		{
			return this->m_array < rhs.m_array;
		}

		bool operator<= (const Array& rhs) const
		{
			return this->m_array <= rhs.m_array;
		}

		bool operator>  (const Array& rhs) const
		{
			return this->m_array > rhs.m_array;
		}

		bool operator>= (const Array& rhs) const
		{
			return this->m_array >= rhs.m_array;
		}
	};

}
