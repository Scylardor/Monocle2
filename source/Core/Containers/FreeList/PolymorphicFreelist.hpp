// Monocle Game Engine source files - Alexandre Baron

#pragma once

namespace moe
{
	template <class BaseType>
	void PolymorphicFreelist<BaseType>::Clear()
	{
		m_objects.Clear();
		m_nextFreeListSlot = UINT32_MAX;
	}


	template <class BaseType>
	BaseType& PolymorphicFreelist<BaseType>::Lookup(FreelistID id)
	{
		MOE_ASSERT(id.Index() < m_objects.Size());
		return *m_objects[id.Index()].m_obj;
	}


	template <class BaseType>
	const BaseType& PolymorphicFreelist<BaseType>::Lookup(FreelistID id) const
	{
		MOE_ASSERT(id.Index() < m_objects.Size());
		return *m_objects[id.Index()].m_obj;
	}


	template <class BaseType>
	template <class Derived, typename... Args>
	FreelistID PolymorphicFreelist<BaseType>::Add(Args&&... args)
	{
		static_assert(std::is_base_of_v<BaseType, Derived>, "BaseType and Derived are unrelated");

		uint32_t newObjIdx;

		if (m_nextFreeListSlot == UINT32_MAX)
		{
			newObjIdx = (uint32_t)m_objects.Size();
			m_objects.PushBack(new Derived(std::forward<Args>(args)...));
		}
		else
		{
			newObjIdx = m_nextFreeListSlot;
			m_nextFreeListSlot = m_objects[m_nextFreeListSlot].m_nextSlot;
		}

		return FreelistID{ newObjIdx };
	}


	template <class BaseType>
	void PolymorphicFreelist<BaseType>::Remove(FreelistID removedId)
	{
		MOE_ASSERT(removedId.Index() < m_objects.Size());
		ObjectStorage& newFreeSlot = m_objects[removedId.Index()];

		delete newFreeSlot.m_obj;

		newFreeSlot.m_nextSlot = m_nextFreeListSlot;
		m_nextFreeListSlot = removedId.Index();
	}

}
