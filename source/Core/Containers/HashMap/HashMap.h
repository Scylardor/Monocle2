// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include <unordered_map>

namespace moe
{
	// Implementation of a hash map
	// Currently relying on the standard library.
	template <class Key, class Data>
	class HashMap
	{
	public:

		typedef	typename std::unordered_map<Key, Data>::iterator		Iterator;
		typedef	typename std::unordered_map<Key, Data>::const_iterator	ConstIterator;
		typedef	typename std::unordered_map<Key, Data>::value_type		ValueType;
		typedef	typename std::unordered_map<Key, Data>::size_type		SizeType;

		// The Coplien rain dance to make things compile
		HashMap() = default;
		~HashMap() = default;
		HashMap(const HashMap& other) = default;
		HashMap& operator=(const HashMap& other) = default;
		HashMap(HashMap&& other) = default;
		HashMap& operator=(HashMap&& other) = default;
		// ----------------------------------------------

		std::pair<Iterator, bool>	Insert(ValueType&& value)						{ return m_hashmap.insert(std::forward<ValueType>(value)); }
		std::pair<Iterator, bool>	Insert(const ValueType& val)					{ return m_hashmap.insert(val); }
		template <class P>
		std::pair<Iterator, bool>	Insert(P&& val)									{ return m_hashmap.insert(val); }
		Iterator					Insert(ConstIterator hint, const ValueType& val){ return m_hashmap.insert(hint, val); }
		template <class P>
		Iterator					Insert(ConstIterator hint, P&& val)				{ return m_hashmap.insert(hint, val); }
		template <class InputIterator>
		void						Insert(InputIterator first, InputIterator last)	{ m_hashmap.insert(first, last); }
		void						Insert(std::initializer_list<ValueType> il)		{ m_hashmap.insert(il); }

		template <class... Args>
		std::pair<Iterator, bool>	Emplace(Args&&... args) { return m_hashmap.emplace(std::forward<Args>(args)...); }

		Iterator	Erase(ConstIterator position)					{ return m_hashmap.erase(position); }
		SizeType	Erase(const Key& k)								{ return m_hashmap.erase(k); }
		Iterator	Erase(ConstIterator first, ConstIterator last)	{ return m_hashmap.erase(first, last); }


		SizeType	Size() const { return m_hashmap.size(); }


		void	Clear() { m_hashmap.clear(); }


		Iterator		Find(const Key& k)			{ return m_hashmap.find(k); }
		ConstIterator	Find(const Key& k) const	{ return m_hashmap.find(k); }


		Data& operator[] (const Key& k) { return m_hashmap[k]; }
		Data& operator[] (Key&& k) { return m_hashmap[k]; }

		Iterator		Begin()			{ return m_hashmap.begin(); }
		ConstIterator	Begin() const	{ return m_hashmap.begin(); }


		Iterator		End()		{ return m_hashmap.end(); }
		ConstIterator	End() const	{ return m_hashmap.end(); }

		// Range-based for loops interface
		Iterator		begin() { return Begin(); }
		ConstIterator	begin() const { return Begin(); }

		Iterator		end() { return End(); }
		ConstIterator	end() const { return End(); }
		//--------------------------------


	private:
		std::unordered_map<Key, Data>	m_hashmap;

	public:
		// Relational operator soup
		bool operator== (const HashMap& rhs) const
		{
			return m_hashmap == rhs.m_hashmap;
		}

		bool operator!= (const HashMap& rhs) const
		{
			return m_hashmap != rhs.m_hashmap;
		}

	};

}