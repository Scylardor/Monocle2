// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Monocle_Core_Export.h"
#include "Core/Preprocessor/moeAssert.h"

#include <array>

namespace moe
{
	// Implementation of an array
	// Currently relying on the standard library.
	template <class DataType, std::size_t N >
	class Array
	{
	public:

		typedef	typename std::array<DataType, N>::iterator			Iterator;
		typedef	typename std::array<DataType, N>::const_iterator	ConstIterator;
		typedef	typename std::array<DataType, N>::value_type		ValueType;
		typedef	typename std::array<DataType, N>::size_type			SizeType;

		Monocle_Core_API Array() = default;
		Monocle_Core_API ~Array() = default;

		Array(std::initializer_list<DataType> il)
		{
			for (auto elemIt = il.begin(); elemIt != il.end(); ++elemIt)
			{
				m_array[elemIt - il.begin()] = *elemIt;
			}
		}

		size_t	Size() const { return m_array.size(); }

		void	Fill(const ValueType& val) { m_array.fill(val); }

		const DataType*	Data() const { return m_array.data(); }

		Iterator		Begin() noexcept		{ return m_array.begin(); }
		ConstIterator	Begin() const noexcept	{ return m_array.begin(); }

		Iterator		End() noexcept			{ return m_array.end(); }
		ConstIterator	End() const noexcept	{ return m_array.end(); }


		DataType&		operator[](SizeType idx)
		{
			MOE_DEBUG_ASSERT(idx < N);
			return m_array[idx];
		}

		const DataType&	operator[](SizeType idx) const
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
		std::array<DataType, N>	m_array;


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
