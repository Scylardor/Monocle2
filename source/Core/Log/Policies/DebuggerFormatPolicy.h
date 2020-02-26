#ifndef MOE_DEBUGGER_FORMAT_POLICY_H_
#define MOE_DEBUGGER_FORMAT_POLICY_H_

// A logging format-policy used to format messages in a Visual Studio output window message's style.
// What's cool about messages formatted this way is that VS can take you to the file/line by clicking on the message.

#include "Core/Preprocessor/moeDLLVisibility.h"

#ifdef MOE_STD_SUPPORT
#include <string>
#endif

namespace moe
{
    struct LogInfo;

    class MOE_DLL_API DebuggerFormatPolicy
    {
    public:
        static const std::string& Format(std::string& buf, const LogInfo& logInfo);
    };
}


#endif // MOE_DEBUGGER_FORMAT_POLICY_H_