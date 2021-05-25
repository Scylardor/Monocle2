#pragma once

#include "Core/Resource/BaseResource.h"

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
				other.m_rscID = INVALID_ENTRY;

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


		TRsc* operator->()
		{
			return &m_resource;
		}


		template <typename Derived>
		TRsc& As()
		{
			static_assert(std::is_base_of_v<TRsc, Derived>);
			return *static_cast<Derived*>(m_resource);
		}

		template <typename Derived>
		const TRsc& As() const
		{
			static_assert(std::is_base_of_v<TRsc, Derived>);
			return *static_cast<const Derived*>(m_resource);
		}



	protected:

		friend ResourceManager;


		Ref(ResourceManager& mgr, TRsc& rsc, RegistryID id) :
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

		ResourceManager*	m_manager{ nullptr };
		TRsc*				m_resource{ nullptr };
		RegistryID			m_rscID{ INVALID_ENTRY };
	};


}