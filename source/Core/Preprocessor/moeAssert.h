#ifndef MOE_ASSERT_H_
#define MOE_ASSERT_H_

#include "Core/Preprocessor/moeLikely.h"

// Monocle Assert
//---------------

// A debug assert used to break if the expression inside is false.
// Warning: this is used to DEBUG, which means it is STRIPPED in Shipping code.
// Thus, one needs to put extra care to put only additional checking code in the assert,
// leaving expressions we want to run in Shipping outside.

#ifndef MOE_SHIPPING
    #include "Core/Debugger/moeDebugger.h"
    #include "Core/Preprocessor/moeStringize.h"
    #include "Core/Preprocessor/moeDLLVisibility.h"

namespace moe
{
    // A dummy wrapper function whose only purpose is to print an error within MOE_ASSERT.
    MOE_DLL_API bool    AssertError(const char* const file, int line, const char* msg);
}

#define MOE_BREAK(expr) (moe::AssertError(__FILE__, __LINE__, MOE_STRINGIZE(expr) ": ASSERT FAILED!") && moe::DebugBreak())
#define MOE_ASSERT(expr)    (MOE_LIKELY((expr)) || MOE_BREAK(expr))

// The DEBUG Assert is completely stripped in Shipping builds.
// It is only for checks you want to make in production, but you don't want included in the final project.
// Don't put important stuff in them; only debug checks you could live without.
#define MOE_DEBUG_ASSERT(expr) MOE_ASSERT(expr)

#else
    #define MOE_ASSERT(expr) (MOE_LIKELY(expr))
    #define MOE_DEBUG_ASSERT(expr)
#endif // MOE_SHIPPING

#endif // MOE_ASSERT_H_
