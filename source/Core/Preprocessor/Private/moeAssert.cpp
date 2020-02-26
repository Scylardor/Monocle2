#ifndef MOE_SHIPPING

#include "Core/Preprocessor/moeAssert.h"
#include "Core/Log/moeLoggerBase.h"
#include "Core/Log/moeLogMacros.h"

namespace moe
{
    bool    AssertError(const char* const file, int line, const char* msg)
    {
        MOE_DEFAULT_LOG_EXPAND(moe::ChanDebug, moe::SevError, file, line, msg);
        return true; // needed to break
    }
}

#endif // MOE_SHIPPING
