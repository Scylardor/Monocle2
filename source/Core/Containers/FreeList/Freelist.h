// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Containers/Vector/Vector.h"

#include "Core/Misc/Types.h"

#ifdef MOE_STD_SUPPORT
#include <utility> // std::pair
#endif


namespace moe
{
	struct FreelistObject
	{
		uint32_t	m_next{0};
	};

	template <class Object>
	class Freelist;


	struct FreelistID
	{
	public:

		FreelistID() = default;

		FreelistID(uint32_t index) :
			m_index(index)
		{}

		uint32_t	Index() const { return m_index; }

	private:

		uint32_t	m_index{0};
	};





	/**
	 * \brief Implementation of a "lightweight" free list, using a vector to keep objects contiguous.
	 * It uses vector data to store indexes to the next free slot in the list.
	 * It does not use any kind of "generation" ID, so extra care should be taken to not keep old dangling IDs
	 * (or an old ID referring to a deleted object could address a new, different object now living at the same index).
	 * \tparam Object the type of object stored in the free list
	 */
	template <class Object>
	class Freelist
	{
	private:

	/**
	 * \brief Union struct to store the freelist objects
	 * I need to store a u32 in the object memory when the slot is "freed" to indicate where is the next slot.
	 * So an union allows to do just that : when the slot is occupied, use the Object. When it's freed, set the next slot.
	 */
	union ObjectStorage
	{
		~ObjectStorage()
		{
			m_obj.~Object();
		}

		// Variadic template constructor is needed for ObjectStorage to work with EmplaceBack and all kinds of objects.
		template <typename... Args>
		ObjectStorage(Args&&... args) :
			m_obj(std::forward<Args>(args)...)
		{}

		// Doesn't compile if I don't add this conversion operator.
		operator Object() { return m_obj; }

		Object	m_obj;	// The stored object
		uint32_t m_nextSlot{ UINT32_MAX };	// The previous value of next freelist slot when this slot gets freed
	};

	public:
		Freelist() = default;

		Freelist(uint32_t numReserved)
		{
			Reserve(numReserved);
		}

		void	Reserve(uint32_t numReserved)
		{
			m_objects.Reserve(numReserved);
		}

		void Clear();

		Object& Lookup(FreelistID id);

		const Object& Lookup(FreelistID id) const;

		template <typename... Args>
		FreelistID Add(Args&&... args);

		void Remove(FreelistID removedId);

	private:
		uint32_t	m_nextFreeListSlot = UINT32_MAX;

		Vector<ObjectStorage>	m_objects;
	};
}

#include "Freelist.hpp"