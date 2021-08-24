// Monocle source files - Alexandre Baron

#pragma once

#include "Core/Containers/Vector/Vector.h"
#include "GameFramework/Service/Service.h"

#include "Core/Log/moeLog.h"

namespace moe
{
	class LogService : public Service
	{
	public:

		using ServiceLocatorType = LogService;

		LogService(Engine& ownerEngine) :
			Service(ownerEngine)
		{}


		template <typename TLog, typename... Ts>
		TLog& EmplaceLogger(Ts&&... args);


		void RemoveLogger(ILogger& toRemove);

	private:

		Vector<std::unique_ptr<ILogger>>	m_loggers;
	};


}

#include "LogService.inl"