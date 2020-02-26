#ifndef MOE_LIKELY_H_
#define MOE_LIKELY_H_

// Macros wrapping branch-prediction hints. Those currently exist only on GCC/Clang.
// Be careful when using those, they can hurt performance instead of improving it
// if used incorrectly. When in doubt, always put the most probable case of an if first,
// not in the else.
// http://stackoverflow.com/questions/3702903/portable-branch-prediction-hints
#if defined(MOE_LINUX) && ( defined(__clang__) || defined(__GNUC__) )
    #define MOE_LIKELY(x)   __builtin_expect(!!(x), 1)
    #define MOE_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define MOE_LIKELY(x)   (x)
    #define MOE_UNLIKELY(x) (x)
#endif // MOE_LINUX

#endif // MOE_LIKELY_H_