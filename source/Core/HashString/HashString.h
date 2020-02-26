// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include <string>

namespace moe
{
	typedef std::size_t		HashID;

	// "Stealing" this from https://blog.molecular-matters.com/2011/06/24/hashed-strings/
	template <class String>
	class FNV1aHashPolicy
	{
	public:
		static HashID	Hash(const String& str)
		{
			return (Hash(str.c_str()));
		}

		static HashID	Hash(const char* cstr)
		{
			std::size_t hash = s_offset;

			while (*cstr != '\0')
			{
				hash ^= *cstr++;
				hash *= s_prime;
			}

			return hash;
		}

		static const std::size_t	s_offset = 2166136261u;
		static const std::size_t	s_prime = 16777619u;
	};


	template <class String>
	class StdHashPolicy
	{
	public:
		static HashID	Hash(const String& str)
		{
			std::hash<String> hash;
			return hash(str);
		}

		static HashID	Hash(const char* cstr)
		{
			std::hash<const char*> hash;
			return hash(cstr);
		}

	};

	/*
	Implementation of a simple Hash String
	The purpose of a hash string is to be easily used as a hash key in a hash map, for example.
	The origin string is stored in "not shipping" modes for ease of debugging.
	Useful to reference asset IDs, the string constructor is disabled in Shipping mode to only allow using IDs (no useless strings).
	TODO: make a generator to replace all HashString string constructors by their hash value.
	*/
	template <class String = std::string, class HashPolicy = StdHashPolicy<String>>
	class HashStringT
	{
	public:

		HashStringT() = default;

		#ifndef MOE_SHIPPING
		explicit HashStringT(const String& str) :
			m_str(str)
		{
			m_id = HashPolicy::Hash(m_str);
		}

		explicit HashStringT(const char* str) :
			m_str(str)
		{
			m_id = HashPolicy::Hash(m_str);
		}
		#endif

		explicit HashStringT(HashID id) :
			m_id(id)
		{}

		HashStringT(const HashStringT & other)
		{
			m_id = other.m_id;
			#ifndef MOE_SHIPPING
			m_str = other.m_str;
			#endif
		}

		explicit HashStringT(HashStringT&& other)
		{
			m_id = other.m_id;
			other.m_id = 0;

			#ifndef MOE_SHIPPING
			m_str = std::move(other.m_str);
			#endif
		}

		HashStringT& operator=(const HashStringT& rhs)
		{
			m_id = rhs.m_id;

			#ifndef MOE_SHIPPING
			m_str = rhs.m_str;
			#endif

			return *this;
		}

		HashStringT& operator=(HashStringT&& rhs)
		{
			m_id = rhs.m_id;
			rhs.m_id = 0;

			#ifndef MOE_SHIPPING
			m_str = std::move(rhs.m_str);
			#endif

			return *this;
		}

		#ifndef MOE_SHIPPING
		HashStringT& operator=(const char* str)
		{
			m_str = str;
			m_id = HashPolicy::Hash(m_str);
			return *this;
		}
		#endif



		// Necessary for HashString to be used as a key in an HashMap, for example
		HashID	operator()() const
		{
			return m_id;
		}


		void	Set(HashID id)
		{
			m_id = id;
		}


		void	Clear()
		{
			m_id = 0;
			#ifndef MOE_SHIPPING
			m_str.clear();
			#endif
		}


		// The string representation of a hash string ID should only be used in not-Shipping mode.
		#ifndef MOE_SHIPPING
		HashID SetString(const String& str)
		{
			m_str = str;
			m_id = HashPolicy::Hash(str);
			return m_id;
		}

		HashID SetString(const char* str)
		{
			m_str = str;
			m_id = HashPolicy::Hash(str);
			return m_id;
		}

		const String&	GetString() const
		{
			return m_str;
		}
		#endif


		// Relational operators soup
		bool	operator==(const HashStringT& rhs) const
		{
			bool eq = (m_id == rhs.m_id);
			return eq;
		}

		bool	operator!=(const HashStringT& rhs) const
		{
			return !(*this == rhs);
		}

		bool	operator<(const HashStringT& rhs) const
		{
			return (m_id < rhs.m_id);
		}

		bool	operator<=(const HashStringT& rhs) const
		{
			return !(*this < rhs);
		}

		bool	operator>(const HashStringT& rhs) const
		{
			return (rhs < *this);
		}

		bool	operator>=(const HashStringT& rhs) const
		{
			return !(*this < rhs);
		}


	private:
		#ifndef MOE_SHIPPING
		String	m_str;
		#endif

		HashID	m_id = 0;
	};


	// Generic HashString type
	typedef HashStringT<>	HashString;
}


#ifdef MOE_STD_SUPPORT
namespace std
{
	// With std support, Monocle HashMap relies on std::unordered_map.
	// In order to use a custom type as a unordered_map key you need to specialize the std::hash template.
	// With this in place, you can instantiate a std::unordered_map using HashString as the key:
	// it will automatically use std::hash<HashString> for the hash value calculations.
	template <class String, class HashPolicy>
	struct hash<moe::HashStringT<String, HashPolicy>>
	{
		std::size_t operator()(const moe::HashStringT<String, HashPolicy>& key) const
		{
			return key();
		}
	};
}
#endif