// Monocle source files - Alexandre Baron

#pragma once
#include "GameFramework/Service/Service.h"

namespace moe
{
	class TimeService : public Service
	{
	public:
		using ServiceLocatorType = TimeService;


		TimeService(Engine& ownerEngine);
	};
}

