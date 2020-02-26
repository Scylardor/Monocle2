#ifndef MOE_NO_FORMAT_POLICY_H_
#define MOE_NO_FORMAT_POLICY_H_

// A logging format-policy that as the name suggests, doesn't add any additional formatting to the original message.

#ifdef MOE_STD_SUPPORT
#include <string>
#endif // MOE_STD_SUPPORT

#include "Core/Preprocessor/moeDLLVisibility.h"

namespace moe
{
    struct LogInfo;

    class MOE_DLL_API NoFormatPolicy
    {
	public:
        static const std::string& Format(std::string& buffer, const LogInfo& logInfo);
    };
}

#endif // MOE_NO_FORMAT_POLICY_H_