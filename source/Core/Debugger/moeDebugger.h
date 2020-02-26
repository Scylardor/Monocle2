#ifndef MOE_DEBUGGER_H_
#define MOE_DEBUGGER_H_

#ifndef MOE_SHIPPING

#include "Core/Preprocessor/moeDLLVisibility.h"

namespace moe
{
    bool                IsDebuggerPresent();

    // DebugBreak returns false only so ASSERT can be used in an if...
    bool    MOE_DLL_API DebugBreak();
}

#endif // MOE_SHIPPING

#endif // MOE_DEBUGGER_H_
