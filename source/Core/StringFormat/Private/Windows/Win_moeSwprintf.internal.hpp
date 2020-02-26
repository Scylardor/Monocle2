// WINDOWS version

#include "Core/Preprocessor/moeAssert.h"

namespace moe
{
    template <typename... Args>
    int SnPrintf(wchar_t * const buffer, std::size_t bufferSize, const wchar_t * const format, const Args& ... args)
    {
        // Unlike snprintf, you need to pass nullptr as buffer to swprintf to get the formatted output size,
        // otherwise if it doesn't fit it returns -1. So we can try first but else we need to call it again to get the wanted size.
        // cf. http://www.cplusplus.com/reference/cwchar/swprintf/
        int result = std::swprintf(buffer, bufferSize, format, StringFormatArg(args)...);
        if (result == -1)
        {
            result = std::swprintf(nullptr, 0, format, StringFormatArg(args)...);
            MOE_ASSERT(result != -1);
        }
        return result;
    }

}