#include "Core/Misc/moeAbort.h"

#include <cstdlib>

namespace moe
{
    void Abort()
    {
        std::abort();
    }
}