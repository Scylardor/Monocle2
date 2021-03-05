#pragma once

#include "ResourceFactory.h"

#include <deque>


/* This class uses a deque as it makes it easy to add new elements without invalidating references to previously existing objects. */
template <class CRTP, class ResourceType>
class AResourceFactory : public CRTP, IResourceFactory
{
	static_assert(std::is_base_of_v<IResource, ResourceType>, "ResourceType must be a subclass of IResource interface.");

public:

	template <typename... Args>
	ResourceType*	CreateResource(Args&&... args)
	{
		ResourceType newRsc = static_cast<CRTP*>(this)->BuildResource(std::forward<Args>(args)...);
		m_resources.emplace_back(std::move(newRsc));
		return &m_resources.back();
	}


protected:



private:


	std::deque<ResourceType>	m_resources;

};