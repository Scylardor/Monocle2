#include "Core/Log/Policies/NoFilterPolicy.h"
#include "Core/Log/LogUtils.h"

namespace moe
{
    NoFilterPolicy::NoFilterPolicy()
    {

    }

    bool NoFilterPolicy::Filter(const LogInfo&)
    {
        return true;
    }

}