#pragma once

#include "Core/Containers/ObjectPool/ObjectPool.h"

namespace moe
{
	using RegistryID = uint32_t;
	const auto INVALID_ENTRY = (RegistryID)~0;

	template <typename TEntry>
	struct RegistryEntry
	{
		RegistryEntry() = default;

		template <typename... Ts>
		RegistryEntry(Ts&&... args) :
			Asset(std::forward<Ts>(args)...),
			ReferenceCount(1)
		{}


		void		SetPersistent(bool isPersistent)
		{
			IsPersistent = isPersistent;
		}

		TEntry		Asset;
		uint32_t	ReferenceCount = 0;
		bool		IsPersistent = false; // If true, won't get deleted even if reference count drops to 0.
	};


	class IRegistry
	{

	};


	/*
	 * A simple template class aimed at storing reference-counted assets.
	 */
	template <typename TEntry, typename Container = DynamicObjectPool<RegistryEntry<TEntry>>>
	class ObjectRegistry : public IRegistry
	{
	public:


		ObjectRegistry(uint32_t reserved = 0) :
			m_registry(reserved)
		{}


		template <typename... Ts>
		[[nodiscard]] RegistryID	EmplaceEntry(Ts&&... args)
		{
			return m_registry.Emplace(std::forward<Ts>(args)...);
		}

		template <typename... Ts>
		[[nodiscard]] RegistryID EmplacePersistentEntry(Ts&&... args);

		void	DeleteEntry(RegistryID id)
		{
			m_registry.Free(id);
		}


		uint32_t	IncrementReference(RegistryID id);


		bool		DecrementReference(RegistryID id);

		[[nodiscard]] uint32_t GetReferenceCount(RegistryID id) const
		{
			return m_registry.Get(id).ReferenceCount;
		}


		[[nodiscard]] TEntry& MutEntry(RegistryID id)
		{
			auto& entry = m_registry.Mut(id);
			return entry.Asset;
		}

		[[nodiscard]] const TEntry& GetEntry(RegistryID id) const
		{
			auto& entry = m_registry.Get(id);
			return entry.Asset;
		}

		[[nodiscard]] auto	Size() const
		{
			return m_registry.Size();
		}


		[[nodiscard]] bool IsPersistent(RegistryID id) const
		{
			return m_registry.Get(id).IsPersistent;
		}



	private:

		Container	m_registry;
	};


}

#include "ObjectRegistry.inl"