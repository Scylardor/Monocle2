#include "LogService.h"

namespace moe
{

	void LogService::RemoveLogger(ILogger& toRemove)
	{
		auto logIt = std::find_if(m_loggers.begin(), m_loggers.end(), [&toRemove](const auto& logPtr)
		{
			return logPtr.get() == &toRemove;
		});

		if (logIt != m_loggers.end())
			m_loggers.EraseBySwap(logIt);
	}
}
