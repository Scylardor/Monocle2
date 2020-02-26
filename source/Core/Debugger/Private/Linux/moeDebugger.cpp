#ifndef MOE_SHIPPING

#include <cstdio>   // FILE* ops
#include <cstring>  // strstr
#include <cstdlib>  // atoi
#include "Core/Debugger/moeDebugger.h"

// Linux version of Monocle debugger utilities

namespace moe
{
    // The idea on Linux is to read the status file of current process and see if a tracer PID is present in it (as GDB would write in it).
    // Only checks the debugger presence once, then returns a static flag value to avoid most of the performance hit of I/O.
    // Credits to Sam Liao on http://stackoverflow.com/questions/3596781/how-to-detect-if-the-current-process-is-being-run-by-gdb
    bool    IsDebuggerPresent()
    {
        // -1 = default value, 0: no debugger, 1: debugger
        static char presenceCheckedFlag = (-1);
        if (presenceCheckedFlag != (-1))
        {
            return bool(presenceCheckedFlag);
        }

        std::FILE* statusFileHandle = std::fopen("/proc/self/status", "r");
        if (statusFileHandle == nullptr)
        {
            return false;
        }

        char buf[256]; // arbitrary size
        std::size_t numRead = std::fread(buf, sizeof(char), sizeof(buf), statusFileHandle);
        bool debuggerPresent = false;

        if (numRead > 0)
        {
            buf[numRead] = '\0';

            static const char TracerPIDHeader[] = "TracerPid:";
            const char * tracerPIdSection = std::strstr(buf, TracerPIDHeader);

            if (tracerPIdSection != nullptr)
            {
                int tracerPID = std::atoi(tracerPIdSection + sizeof(TracerPIDHeader) - 1);
                debuggerPresent = (tracerPID != 0);
            }
        }

        std::fclose(statusFileHandle);

        presenceCheckedFlag = char(debuggerPresent);
        return debuggerPresent;
    }


    // Generates an interrupt 3 (commonly raises SIGTRAP caught by the debugger without crashing the program afterwards).
    bool    DebugBreak()
    {
        #ifndef MOE_PROFILE // We don't want an assert break ruining a profiling session
        if (IsDebuggerPresent())
        {
            __asm__("int $3");
        }
        #endif // MOE_PROFILE

        return false;
    }
}

#endif // MOE_SHIPPING
