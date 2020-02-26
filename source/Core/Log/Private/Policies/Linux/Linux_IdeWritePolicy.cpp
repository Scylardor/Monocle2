#include "Core/Log/Policies/IdeWritePolicy.h"
#include <iostream>

namespace moe
{

    IdeWritePolicy::IdeWritePolicy() :
        m_redirection(std::cerr)
    {
    }

    void    IdeWritePolicy::Write(const std::string& message)
    {
        m_redirection.Write(message);
    }
}
