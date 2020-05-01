// Monocle Game Engine source files - Alexandre Baron

#pragma once

namespace moe
{
	template <class Object>
	void Freelist<Object>::Clear()
	{
		m_objects.Clear();
		m_nextFreeListSlot = UINT32_MAX;
	}


	template <class Object>
	Object& Freelist<Object>::Lookup(FreelistID id)
	{
		MOE_ASSERT(id.Index() < m_objects.Size());
		return m_objects[id.Index()].m_obj;
	}


	template <class Object>
	const Object& Freelist<Object>::Lookup(FreelistID id) const
	{
		MOE_ASSERT(id.Index() < m_objects.Size());
		return m_objects[id.Index()].m_obj;
	}


	template <class Object>
	template <typename ... Args>
	FreelistID Freelist<Object>::Add(Args&&... args)
	{
		uint32_t newObjIdx;

		if (m_nextFreeListSlot == UINT32_MAX)
		{
			newObjIdx = (uint32_t)m_objects.Size();
			m_objects.EmplaceBack(std::forward<Args>(args)...);
		}
		else
		{
			newObjIdx = m_nextFreeListSlot;
			m_nextFreeListSlot = m_objects[m_nextFreeListSlot].m_nextSlot;
		}

		return FreelistID{ newObjIdx };
	}


	template <class Object>
	void Freelist<Object>::Remove(FreelistID removedId)
	{
		MOE_ASSERT(removedId.Index() < m_objects.Size());
		ObjectStorage& newFreeSlot = m_objects[removedId.Index()];
		newFreeSlot.m_obj.~Object();
		newFreeSlot.m_nextSlot = m_nextFreeListSlot;
		m_nextFreeListSlot = removedId.Index();
	}

}