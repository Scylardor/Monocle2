#pragma once

#include "ResourceFactory.h"

#include "Core/Misc/moeCRTP.h"

#include "GameFramework/Resources/Resource/Resource.h"


namespace moe
{
	/* This class uses a deque as it makes it easy to add new elements without invalidating references to previously existing objects. */
	template <class Top, class TRsc>
	class AResourceFactory : public CRTP<Top>, public IResourceFactory
	{
		//static_assert(std::is_base_of_v<IResource, ResourceType>, "ResourceType must be a subclass of IResource interface.");

	public:

		template <typename... Args>
		Resource<TRsc> CreateResource(Args&&... args)
		{
			Resource<TRsc> resource = MOE_CRTP_IMPL(BuildResource, std::forward<Args>(args)...);
			return resource;
		}


	protected:



	private:

		ObjectRegistry<TRsc>	m_registry;

	};


}

