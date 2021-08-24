// Monocle source files - Alexandre Baron

#pragma once
#include "Core/Containers/ObjectPool/ObjectPool.h"
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
		TLog& EmplaceLogger(Ts&&... args)
		{
			static_assert(std::is_base_of_v<ILogger, TLog>, "Can only work with ILogger derived classes");
			m_loggers.EmplaceBack(std::make_unique<TLog>(std::forward<Ts>(args)...));

			m_loggers.Back().get()->LinkTo(GetLogChainSingleton());

			return static_cast<TLog&>(*m_loggers.Back());
		}


		void RemoveLogger(ILogger& toRemove);

	private:

		Vector<std::unique_ptr<ILogger>>	m_loggers;
	};
}

