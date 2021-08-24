#ifndef MOE_LOGGER_BASE_H_
#define MOE_LOGGER_BASE_H_

#ifdef MOE_STD_SUPPORT

#include "Core/Containers/IntrusiveListNode.h"
#include "Core/Log/LogUtils.h"

#include "Monocle_Core_Export.h"

namespace moe
{
    // The ILogger is a logger interface you can derive from to implement your own policy-based implementations.
    // Making it a virtual class so they can be linked by intrusive list nodes (not possible with templates)
    // Using STD support because the Log function uses std::string as a format buffer.
    class ILogger : public IntrusiveListNode<ILogger>
    {
    public:
        ILogger()
    	: IntrusiveListNode<ILogger>(this)
    	{}

        virtual ~ILogger() = default;

        // A virtual function cannot be template so preformat the message and call the virtual function afterwards for further processing
        template <typename... Args>
        void    Log(LogChannel channel, LogSeverity severity, const char* file, int line, const char* format, const Args&... args);

    protected:
        virtual void    ConsumeLogMessage(const LogInfo& logInfo) = 0;
    };


    Monocle_Core_API ILogger&  GetLogChainSingleton();
}

#include "Core/Log/Private/ILogger.internal.hpp"


#endif // MOE_STD_SUPPORT


#endif // MOE_LOGGER_BASE_H_
