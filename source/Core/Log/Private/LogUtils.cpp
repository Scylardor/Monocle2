#include "Core/Log/LogUtils.h"
#include "Core/Misc/moeCountof.h"
#include "Core/Preprocessor/moeAssert.h"

namespace moe
{
    namespace internals
    {
        static const char*  LogChannelStrings[] =
        {
            "Default",
            "Debug",
            "Windowing",
            "Graphics",
			"Input"
            // ...
        };
        static_assert(moe::Countof(LogChannelStrings) == moe::LogChannel::_LogChannelMax_, "Each LogChannel value must have a matching string representation");

        static const char*  LogSeverityStrings[] =
        {
            "INFO",
            "WARNING",
            "ERROR",
            "FATAL"
        };
        static_assert(moe::Countof(LogSeverityStrings) == moe::LogSeverity::_LogSeverityMax_, "Each LogSeverity value must have a matching string representation");

        const char* GetChannelString(LogChannel chan)
        {
            MOE_DEBUG_ASSERT(chan < LogChannel::_LogChannelMax_);
            return LogChannelStrings[(int)chan];
        }

        const char* GetSeverityString(LogSeverity sev)
        {
            MOE_DEBUG_ASSERT(sev < LogSeverity::_LogSeverityMax_);
            return LogSeverityStrings[(int)sev];
        }
    }
}