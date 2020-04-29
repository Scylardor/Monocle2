#ifndef MOE_OUT_STREAM_WRITE_POLICY_H_
#define MOE_OUT_STREAM_WRITE_POLICY_H_

#ifdef MOE_STD_SUPPORT
#include <iostream>
#include <string>
#include "Monocle_Core_Export.h"

// A logging write policy that writes using an output std IO stream.
namespace moe
{
    class OutStreamWritePolicy
    {
    public:
        Monocle_Core_API OutStreamWritePolicy(std::ostream& outStream = std::cout);
        Monocle_Core_API ~OutStreamWritePolicy();

        Monocle_Core_API void    Write(const std::string& message);

    private:
        std::ostream&   m_ostream;
    };
}
#endif // MOE_STD_SUPPORT

#endif // MOE_OUT_STREAM_WRITE_POLICY_H_