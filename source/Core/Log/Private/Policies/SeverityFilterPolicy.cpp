#include "Core/Log/Policies/SeverityFilterPolicy.h"

namespace moe
{
    SeverityFilterPolicy::SeverityFilterPolicy(LogSeverity filterSeverity) :
        m_minimumSeverity(filterSeverity)
    {
    }

    bool    SeverityFilterPolicy::Filter(const LogInfo& logInfo)
    {
        return ((int)logInfo.severity >= (int)m_minimumSeverity);
    }

    void    SeverityFilterPolicy::SetFilterSeverity(LogSeverity newMinSeverity)
    {
        m_minimumSeverity = newMinSeverity;
    }

    LogSeverity SeverityFilterPolicy::GetFilterSeverity() const
    {
        return m_minimumSeverity;
    }
}
