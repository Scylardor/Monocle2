#pragma once

namespace moe
{
	template <typename TEntry, typename Container>
	template <typename ... Ts>
	RegistryID ObjectRegistry<TEntry, Container>::EmplacePersistenEntry(Ts&&... args)
	{
		auto id = m_registry.Emplace(std::forward<Ts>(args)...);

		auto& entry = m_registry.Mut(id);
		entry.SetPersistent(true);

		return id;
	}

	template <typename TEntry, typename Container>
	uint32_t ObjectRegistry<TEntry, Container>::IncrementReference(RegistryID id)
	{
		auto& entry = m_registry.Mut(id);
		++entry.ReferenceCount;
		return entry.ReferenceCount;
	}


	template <typename TEntry, typename Container>
	bool ObjectRegistry<TEntry, Container>::DecrementReference(RegistryID id)
	{
		auto& entry = m_registry.Mut(id);
		auto refCount = entry.ReferenceCount--;

		const bool isToBeDeleted = (refCount == 0 && !entry.IsPersistent);
		if (isToBeDeleted)
			DeleteEntry(id);

		return refCount;
	}
}