#include "Core/Log/Policies/DebuggerFormatPolicy.h"
#include "Core/StringFormat/moeStringFormat.h"
#include "Core/Log/LogUtils.h"

namespace moe
{
    const std::string&    DebuggerFormatPolicy::Format(std::string& buf, const LogInfo& logInfo)
    {
        StringFormat(buf, "%s(%d): [%s] (%s) %s",
            logInfo.file,
            logInfo.line,
            internals::GetChannelString(logInfo.channel),
            internals::GetSeverityString(logInfo.severity),
            logInfo.message.c_str());

        return buf;
    }
}