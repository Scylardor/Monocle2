#pragma once

#include "Core/Resource/Resource.h"

#include "Core/Containers/AssetRegistry/ObjectRegistry.h"

namespace moe
{

	class ResourceManager;

	template <typename TRsc>
	struct Ref
	{
		static_assert(std::is_base_of_v<IBaseResource, TRsc>);

	public:

		Ref() = default;


		Ref(const Ref& other)
		{
			*this = other;
		}

		Ref& operator=(const Ref& other)
		{
			if (this != &other)
			{
				Decrement();

				m_manager = other.m_manager;
				m_resource = other.m_resource;
				m_rscID = other.m_rscID;

				Increment();
			}

			return *this;
		}



		Ref(Ref&& other) noexcept
		{
			*this = std::move(other);
		}

		Ref& operator=(Ref&& other) noexcept
		{
			if (this != &other)
			{
				Decrement();

				m_manager = other.m_manager;
				m_resource = other.m_resource;
				m_rscID = other.m_rscID;
				other.m_rscID = INVALID_ENTRY;
			}

			return *this;
		}



		~Ref()
		{
			Decrement();
		}


		RegistryID	ID() const
		{
			return m_rscID;
		}


		void	Reset()
		{
			*this = Ref();
		}

		template <typename Derived>
		Derived& As()
		{
			static_assert(std::is_base_of_v<TRsc, Derived>);
			return *static_cast<Derived*>(m_resource);
		}

		template <typename Derived>
		const Derived& As() const
		{
			static_assert(std::is_base_of_v<TRsc, Derived>);
			return *static_cast<const Derived*>(m_resource);
		}


		TRsc* operator->()
		{
			return m_resource;
		}

		const TRsc* operator->() const
		{
			return m_resource;
		}

		TRsc& operator*()
		{
			return *m_resource;
		}

		const TRsc& operator*() const
		{
			return *m_resource;
		}

		operator bool() const
		{
			return (m_rscID != INVALID_ENTRY);
		}


	protected:

		friend ResourceManager;


		Ref(IResourceManager& mgr, TRsc& rsc, RegistryID id) :
			m_manager(&mgr),
			m_resource(&rsc),
			m_rscID(id)
		{
			Increment();
		}


		void	Increment()
		{
			if (m_rscID != INVALID_ENTRY)
			{
				MOE_ASSERT(m_manager);
				m_manager->IncrementReference(m_rscID);
			}
		}

		void	Decrement()
		{
			if (m_rscID != INVALID_ENTRY)
			{
				MOE_ASSERT(m_manager);
				m_manager->DecrementReference(m_rscID);
			}
		}




	private:
		// TODO : Maybe couple ResourceRef and ResourceManager together
		// so that we don't have to rely on this IResourceManager virtual abstraction
		IResourceManager*	m_manager{ nullptr };
		TRsc*				m_resource{ nullptr };
		RegistryID			m_rscID{ INVALID_ENTRY };
	};


}