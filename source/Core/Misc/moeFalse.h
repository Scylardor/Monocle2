#ifndef MOE_FALSE_H_
#define MOE_FALSE_H_

#pragma once

#include <type_traits>

// moe::False is a dummy type you can use in statically-types contexts to always yield a compile-time false value.
// This can be useful e.g. in type-dependent static assertion contexts, where you need an always-false assert but it needs
// to refer to a parameter type (in templates made by GCC for example).
namespace moe
{
    template<typename T>
    struct False : std::false_type
    { };

}

#endif // MOE_FALSE_H_
