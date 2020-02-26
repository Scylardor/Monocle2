#include "Core/Log/Policies/OutStreamWritePolicy.h"

namespace moe
{
    OutStreamWritePolicy::OutStreamWritePolicy(std::ostream& outStream) :
        m_ostream(outStream)
    {
    }

    OutStreamWritePolicy::~OutStreamWritePolicy()
    {
        m_ostream.flush();
    }

    void    OutStreamWritePolicy::Write(const std::string& message)
    {
        m_ostream << message.c_str() << std::endl;
    }
}
