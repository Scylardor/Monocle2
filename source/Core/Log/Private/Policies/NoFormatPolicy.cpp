#include "Core/Log/Policies/NoFormatPolicy.h"
#include "Core/StringFormat/moeStringFormat.h"
#include "Core/Log/LogUtils.h"

namespace moe
{
    const std::string& NoFormatPolicy::Format(std::string&, const LogInfo& logInfo)
    {
        return logInfo.message;
    }
}