#ifndef MOE_OUT_STREAM_WRITE_POLICY_H_
#define MOE_OUT_STREAM_WRITE_POLICY_H_

#ifdef MOE_STD_SUPPORT
#include <iostream>
#include <string>
#include "Core/Preprocessor/moeDLLVisibility.h"

// A logging write policy that writes using an output std IO stream.
namespace moe
{
    class OutStreamWritePolicy
    {
    public:
        MOE_DLL_API OutStreamWritePolicy(std::ostream& outStream = std::cout);
        MOE_DLL_API ~OutStreamWritePolicy();

        MOE_DLL_API void    Write(const std::string& message);

    private:
        std::ostream&   m_ostream;
    };
}
#endif // MOE_STD_SUPPORT

#endif // MOE_OUT_STREAM_WRITE_POLICY_H_