// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Preprocessor/moeAssert.h"
#include <vector>


namespace moe
{
	// Implementation of a vector
	// Currently relying on the standard library.
	template <class DataType>
	class Vector
	{
	public:

		typedef	typename std::vector<DataType>::iterator		Iterator;
		typedef	typename std::vector<DataType>::const_iterator	ConstIterator;
		typedef	typename std::vector<DataType>::value_type		ValueType;
		typedef	typename std::vector<DataType>::size_type		SizeType;

		// The Coplien rain dance to make things compile
		explicit Vector() = default;
		explicit Vector(SizeType n) :
			m_vec(n) {}
		Vector(SizeType n, const ValueType& val) :
			m_vec(n, val) {}
		template <class InputIterator>
		Vector(InputIterator first, InputIterator last) :
			m_vec(first, last) {}
		Vector(std::initializer_list<ValueType> il) :
			m_vec(il) {}

		~Vector() = default;
		Vector(const Vector& other) = default;
		Vector& operator=(const Vector& other) = default;
		Vector(Vector&& other) = default;
		Vector& operator=(Vector&& other) = default;
		// ----------------------------------------------

		void	PushBack(const ValueType& val)	{ m_vec.push_back(val); }
		void	PushBack(ValueType&& val)		{ m_vec.push_back(std::forward<ValueType>(val)); }

		template <class... Args>
		Iterator	Emplace(ConstIterator position, Args&&... args)	{ return m_vec.emplace(position, std::forward<Args>(args)...); }
		template <class... Args>
		void		EmplaceBack(Args&&... args)	{ m_vec.emplace_back(std::forward<Args>(args)...); }

		void	PopBack() { return m_vec.pop_back(); }



		Iterator	Insert(ConstIterator position, const ValueType& val)					{ return m_vec.insert(position, val); }
		Iterator	Insert(ConstIterator position, SizeType n, const ValueType& val)		{ return m_vec.insert(position, n, val); }
		template <class InputIterator>
		Iterator	Insert(ConstIterator position, InputIterator first, InputIterator last)	{ return m_vec.insert(position, first, last); }
		Iterator	Insert(ConstIterator position, ValueType&& val)							{ return m_vec.insert(position, std::forward<ValueType>(val)); }
		Iterator	Insert(ConstIterator position, std::initializer_list<ValueType> il)		{ return m_vec.insert(position, il); }


		Iterator	Erase(ConstIterator position)					{ return m_vec.erase(position); }
		Iterator	Erase(ConstIterator first, ConstIterator last)	{ return m_vec.erase(first, last); }


		/*	Swaps the pointed Iterator with the last one, then does a PopBack. This doesn't preserve the vector order!
		Returns the Iterator pointing to the new element moved in place of the erased one. */
		Iterator	EraseBySwap(Iterator erasedIt)
		{
			SizeType position = erasedIt - Begin();
			std::iter_swap(erasedIt, End() - 1);
			PopBack();
			return (Begin() + position);
		}

		/*	Swaps the pointed element with the last one, then does a PopBack. This doesn't preserve the vector order!
			Returns the Iterator pointing to the new element moved in place of the erased one. */
		Iterator	EraseBySwapAt(SizeType position)
		{
			MOE_DEBUG_ASSERT(position < Size());
			return (EraseBySwap(Begin() + position));
		}


		SizeType	Size() const noexcept	{ return m_vec.size(); }
		SizeType	Capacity() const noexcept	{ return m_vec.capacity(); }

		bool	Empty() const noexcept { return m_vec.empty(); }

		void	Clear() { m_vec.clear(); }

		void	Reserve(SizeType n) { return m_vec.reserve(n); }

		void	Resize(SizeType n)							{ return m_vec.resize(n); }
		void	Resize(SizeType n, const ValueType& val)	{ return m_vec.resize(n, val); }

		ValueType*			Data() noexcept			{ return m_vec.data(); }
		const ValueType*	Data() const noexcept	{ return m_vec.data(); }


		DataType&		operator[](SizeType idx)		{ return m_vec[idx]; }
		const DataType&	operator[](SizeType idx) const	{ return m_vec[idx]; }


		Iterator		Begin() noexcept		{ return m_vec.begin(); }
		ConstIterator	Begin() const noexcept	{ return m_vec.begin(); }

		Iterator		End() noexcept			{ return m_vec.end(); }
		ConstIterator	End() const noexcept	{ return m_vec.end(); }


		// C++11 range-based for loops interface
		Iterator		begin() { return Begin(); }
		ConstIterator	begin() const { return Begin(); }

		Iterator		end() { return End(); }
		ConstIterator	end() const { return End(); }
		//--------------------------------

	private:

		std::vector<DataType>	m_vec;

	public:
		// Relational operators soup
		bool operator== (const Vector& rhs) const
		{
			return this->m_vec == rhs.m_vec;
		}

		bool operator!= (const Vector& rhs) const
		{
			return this->m_vec != rhs.m_vec;
		}

		bool operator<  ( const Vector& rhs) const
		{
			return this->m_vec < rhs.m_vec;
		}

		bool operator<= (const Vector& rhs) const
		{
			return this->m_vec <= rhs.m_vec;
		}

		bool operator>  (const Vector& rhs) const
		{
			return this->m_vec > rhs.m_vec;
		}

		bool operator>= (const Vector& rhs) const
		{
			return this->m_vec >= rhs.m_vec;
		}

	};


}

