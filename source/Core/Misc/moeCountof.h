#ifndef MOE_COUNTOF_H_
#define MOE_COUNTOF_H_

#include <cstddef>

// A C++11 version of countof, to get the size of static arrays.
namespace moe
{
    template <typename T, std::size_t N>
    constexpr std::size_t Countof(T (&)[N]) noexcept
    {
        return N;
    }
}


#endif // MOE_COUNTOF_H_
