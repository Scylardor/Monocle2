#pragma once

#include "GameFramework/Resources/Resource/Resource.h"

#include "Monocle_GameFramework_Export.h"

namespace moe
{
	class ResourceManager;

	class Monocle_GameFramework_API IResourceFactory
	{
	public:

		IResourceFactory(ResourceManager* rscMgr = nullptr) :
			m_rscMgr(rscMgr)
		{}

		virtual ~IResourceFactory() {}

		virtual IResource CreateResource() = 0;




	protected:

		virtual IResource	CreateResourceImpl() = 0;

	private:
		ResourceManager*	m_rscMgr = nullptr;

	};

}


