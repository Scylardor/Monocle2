// Monocle source files - Alexandre Baron

#pragma once

#include "GameFramework/Service/Service.h"
#include "Core/Resource/ResourceManager.h"

#include "Core/Log/moeLog.h"

namespace moe
{
	class ResourceService : public Service
	{
	public:

		using ServiceLocatorType = ResourceService;

		ResourceService(Engine& ownerEngine) :
			Service(ownerEngine)
		{}


		template <typename Resource, typename... Args>
		Ref<Resource>	EmplaceResource(const HashString& rscHandle, Args&&... args)
		{
			return m_rscManager.Insert<Resource>(rscHandle, std::forward<Args>(args)...);
		}



	private:

		ResourceManager	m_rscManager;
	};


}
