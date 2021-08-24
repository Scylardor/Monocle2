#pragma once
namespace moe
{
	template <typename TLog, typename ... Ts>
	TLog& LogService::EmplaceLogger(Ts&&... args)
	{
		static_assert(std::is_base_of_v<ILogger, TLog>, "Can only work with ILogger derived classes");
		m_loggers.EmplaceBack(std::make_unique<TLog>(std::forward<Ts>(args)...));

		m_loggers.Back().get()->LinkTo(GetLogChainSingleton());

		return static_cast<TLog&>(*m_loggers.Back());
	}
}