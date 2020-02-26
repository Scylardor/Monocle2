#include "Core/Log/Policies/CaptureWritePolicy.h"

namespace moe
{
    void    CaptureWritePolicy::Write(const std::string& message)
    {
        m_captured += message;
    }

    void    CaptureWritePolicy::Clear()
    {
        m_captured.clear();
    }

    // Not the best interface. Wish we could return const std::string&. But not across the DLL boundary
    const char*    CaptureWritePolicy::GetCapturedOutput() const
    {
        return m_captured.c_str();
    }
}