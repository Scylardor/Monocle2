#ifndef MOE_SEVERITY_FILTER_POLICY_H_
#define MOE_SEVERITY_FILTER_POLICY_H_

// A logging filter-policy that filters on a given minimum severity.
#include "Monocle_Core_Export.h"
#include "Core/Log/LogUtils.h"

namespace moe
{
    class Monocle_Core_API SeverityFilterPolicy
    {
    public:
        SeverityFilterPolicy(LogSeverity filterSeverity = LogSeverity::SevInfo);

        bool    Filter(const LogInfo& logInfo);
        void    SetFilterSeverity(LogSeverity newMinSeverity);

        LogSeverity GetFilterSeverity() const;

    private:
        LogSeverity m_minimumSeverity;
    };
}

#endif // MOE_SEVERITY_FILTER_POLICY_H_