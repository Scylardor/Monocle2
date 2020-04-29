#ifndef MOE_WIN_IDE_WRITE_POLICY_H_
#define MOE_WIN_IDE_WRITE_POLICY_H_

// WINDOWS version of IdeWritePolicy
// A write policy supposed to map to an IDE's output window.
// On Windows, it uses WinAPI OutputDebugString to output messages to Visual Studio Output window.

#include "Monocle_Core_Export.h"

#ifdef MOE_STD_SUPPORT
#include <string>

namespace moe
{
    class Monocle_Core_API IdeWritePolicy
    {
    public:
        void Write(const std::string& message);
    };
}

#endif // MOE_STD_SUPPORT

#endif // MOE_WIN_IDE_WRITE_POLICY_H_