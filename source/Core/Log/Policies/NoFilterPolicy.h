#ifndef MOE_NO_FILTER_POLICY_H_
#define MOE_NO_FILTER_POLICY_H_

// A logging filter-policy that, as its name says, doesn't filter anything.

#include "Monocle_Core_Export.h"

namespace moe
{
    struct LogInfo;

    class Monocle_Core_API NoFilterPolicy
    {
    public:
        NoFilterPolicy();

        static bool Filter(const LogInfo&);
    };

}

#endif // MOE_NO_FILTER_POLICY_H_