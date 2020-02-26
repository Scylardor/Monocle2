#ifndef MOE_SHIPPING

#include <Windows.h>
#include "Core/Debugger/moeDebugger.h"

// Windows version of Monocle debugger utilities
// (basically a wrapper of windows API)

namespace moe
{
    bool    IsDebuggerPresent()
    {
        // comparing to 0 to avoid warning C4800: 'BOOL' : forcing value to bool 'true' or 'false' (performance warning)
        return (::IsDebuggerPresent() != 0);
    }

    bool DebugBreak()
    {
        #ifndef MOE_PROFILE // We don't want an assert break ruining a profiling session
        if (IsDebuggerPresent())
        {
			__debugbreak();
        }
        #endif // MOE_PROFILE

        return false;
    }
}

#endif // MOE_SHIPPING
