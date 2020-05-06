// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Containers/Vector/Vector.h"

#include "detail/FreeListObject.h"


namespace moe
{


	/**
	 * \brief Implementation of a "lightweight" free list, using a vector to keep objects contiguous.
	 * It uses vector data to store indexes to the next free slot in the list.
	 * It does not use any kind of "generation" ID, so extra care should be taken to not keep old dangling IDs
	 * (or an old ID referring to a deleted object could address a new, different object now living at the same index).
	 * This is a variant of free list that stores base class pointers and can dynamically allocate new derived types of this base class.
	 * \tparam Object the type of object stored in the free list
	 */
	template <typename BaseType>
	class PolymorphicFreelist
	{
	private:

	/**
	 * \brief Union struct to store the freelist objects
	 * I need to store a u32 in the object memory when the slot is "freed" to indicate where is the next slot.
	 * So an union allows to do just that : when the slot is occupied, use the Object. When it's freed, set the next slot.
	 */
	union ObjectStorage
	{
		~ObjectStorage() = default;

		ObjectStorage(BaseType* baseObj) :
			m_obj(baseObj)
		{}

		// Doesn't compile if I don't add this conversion operator.
		operator BaseType*() { return m_obj; }

		BaseType*	m_obj;	// The stored object
		uint32_t	m_nextSlot{ UINT32_MAX };	// The previous value of next freelist slot when this slot gets freed
	};

	public:
		PolymorphicFreelist() = default;

		PolymorphicFreelist(uint32_t numReserved)
		{
			Reserve(numReserved);
		}

		void	Reserve(uint32_t numReserved)
		{
			m_objects.Reserve(numReserved);
		}

		void Clear();

		BaseType& Lookup(FreelistID id);

		const BaseType& Lookup(FreelistID id) const;

		template <typename Derived, typename... Args>
		FreelistID Add(Args&&... args);

		void Remove(FreelistID removedId);

	private:
		uint32_t	m_nextFreeListSlot = UINT32_MAX;

		Vector<ObjectStorage>	m_objects;
	};
}

#include "PolymorphicFreelist.hpp"