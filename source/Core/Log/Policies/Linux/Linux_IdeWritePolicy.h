#ifndef MOE_LINUX_IDE_WRITE_POLICY_H_
#define MOE_LINUX_IDE_WRITE_POLICY_H_

// LINUX version of IdeWritePolicy
// A write policy supposed to map to an IDE's output window.
// On Linux, IdeWritePolicy basically acts as a wrapper around the error fd (std::cerr).

#ifdef MOE_STD_SUPPORT
#include <string>
#include "Core/Preprocessor/moeDLLVisibility.h"
#include "Core/Log/Policies/OutStreamWritePolicy.h"

namespace moe
{
    class IdeWritePolicy
    {
    public:
        MOE_DLL_API IdeWritePolicy();
        MOE_DLL_API void Write(const std::string& message);

    private:
        moe::OutStreamWritePolicy   m_redirection;
    };
}

#endif // MOE_STD_SUPPORT

#endif // MOE_LINUX_IDE_WRITE_POLICY_H_