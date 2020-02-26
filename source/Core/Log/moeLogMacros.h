#ifndef MOE_LOG_MACROS_H_
#define MOE_LOG_MACROS_H_

#include "Core/Log/moeLoggerBase.h"


// The macros commonly used for logging.
// Each macro comes in two flavors: with or without a specified Logger. Without a specified Logger, we use the static default logger.
// The ##__VA_ARGS__ trick is an extension managed by at least MSVC and GCC to take care of "no var args provided" cases.
#define MOE_LOG_TO_LOGGER(logger, channel, severity, file, line, format, ...) \
    logger.Log(channel, severity, file, line, format, ##__VA_ARGS__)

#define MOE_LOG_EXPAND(logger, channel, severity, format, ...) \
    MOE_LOG_TO_LOGGER(logger, channel, severity, __FILE__, __LINE__, format, ##__VA_ARGS__)


#define MOE_DEFAULT_LOG_EXPAND(channel, severity, file, line, format, ...)  \
    MOE_LOG_TO_LOGGER(moe::GetLogChainSingleton(), channel, severity, file, line, format, ##__VA_ARGS__)


#define MOE_DEFAULT_LOG(channel, severity, format, ...) \
    MOE_DEFAULT_LOG_EXPAND(channel, severity, __FILE__, __LINE__, format, ##__VA_ARGS__)


#define MOE_INFO(channel, format, ...) \
    MOE_DEFAULT_LOG(channel, moe::SevInfo, format, ##__VA_ARGS__)

#define MOE_LOGGER_INFO(logger, channel, format, ...) \
    MOE_LOG_EXPAND(logger, channel, moe::SevInfo, format, ##__VA_ARGS__)


#define MOE_WARNING(channel, format, ...) \
    MOE_DEFAULT_LOG(channel, moe::SevWarning, format, ##__VA_ARGS__)

#define MOE_LOGGER_WARNING(logger, channel, format, ...) \
    MOE_LOG_EXPAND(logger, channel, moe::SevWarning, format, ##__VA_ARGS__)


#define MOE_ERROR(channel, format, ...) \
    MOE_DEFAULT_LOG(channel, moe::SevError, format, ##__VA_ARGS__)

#define MOE_LOGGER_ERROR(logger, channel, format, ...) \
    MOE_LOG_EXPAND(logger, channel, moe::SevError, format, ##__VA_ARGS__)


#define MOE_FATAL(channel, format, ...) \
    MOE_DEFAULT_LOG(channel, moe::SevFatal, format, ##__VA_ARGS__)

#define MOE_LOGGER_FATAL(logger, channel, format, ...) \
    MOE_LOG_EXPAND(logger, channel, moe::SevFatal, format, ##__VA_ARGS__)

// MOE_LOG = MOE_INFO, basically
#define MOE_LOG(format, ...) MOE_INFO(moe::ChanDefault, format, ##__VA_ARGS__)

#endif // MOE_LOG_MACROS_H_
