#pragma once

#include "Core/Containers/AssetRegistry/ObjectRegistry.h"

namespace moe
{

	class IResource
	{
	public:

		IResource() = default;

		IResource(IRegistry& registry, RegistryID myID) :
			m_registry(&registry), m_ID(myID)
		{}


		IResource(IResource&& other) noexcept
		{
			*this = std::move(other);
		}

		IResource& operator=(IResource&& other) noexcept
		{
			if (&other == this)
				return *this;

			m_registry = other.m_registry;
			other.m_registry = nullptr;
			MOE_ASSERT(m_registry);

			m_ID = other.m_ID;
			other.m_ID = INVALID_ENTRY;

			return *this;
		}


		[[nodiscard]] RegistryID	ID() const
		{
			return m_ID;
		}

	protected:

		template <typename T>
		ObjectRegistry<T>& Registry()
		{
			MOE_ASSERT(m_registry);
			return static_cast<ObjectRegistry<T>&>(*m_registry);
		}

	private:

		IRegistry*	m_registry{nullptr};
		RegistryID	m_ID{ 0 };
	};


	template <typename T>
	class Resource : public IResource
	{
	public:

		Resource() = default;

		Resource(ObjectRegistry<T>& registry, RegistryID myID) :
			IResource(registry, myID)
		{}


		Resource(const Resource& other)
		{
			*this = other;
		}


		Resource(Resource&& other) noexcept :
			IResource(std::move(other))
		{
		}


		~Resource()
		{
			if (ID() != INVALID_ENTRY)
				Registry<T>().DecrementReference(ID());
		}


		Resource& operator=(const Resource& other)
		{
			if (&other == this)
				return *this;

			if (ID() != INVALID_ENTRY)
				Registry<T>().IncrementReference(ID());

			return *this;
		}



		[[nodiscard]] T* operator->()
		{
			T& object = Registry<T>().MutEntry(ID());
			return &object;
		}


		[[nodiscard]] const T* operator->() const
		{
			const T& object = Registry<T>().GetEntry(ID());
			return &object;
		}

	};

}
