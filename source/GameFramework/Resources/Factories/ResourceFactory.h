#pragma once

#include "GameFramework/Resources/Resource/Resource.h"

#include "Monocle_GameFramework_Export.h"

namespace moe
{
	class ResourceManager;

	class Monocle_GameFramework_API IResourceFactory
	{
	public:

		IResourceFactory(ResourceManager& rscMgr) :
			m_rscMgr(&rscMgr)
		{}

		virtual ~IResourceFactory() {}

		IResource* CreateResource();


	protected:



	private:
		ResourceManager*	m_rscMgr = nullptr;

	};

}


