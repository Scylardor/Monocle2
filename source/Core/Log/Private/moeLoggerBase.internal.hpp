#include "Core/StringFormat/moeStringFormat.h"

namespace moe
{
    template <typename... Args>
    void    LoggerBase::Log(LogChannel channel, LogSeverity severity, const char* file, int line, const char* format, const Args&... args)
    {
        // Preformat the message once for all chained loggers, instead of letting them doing it multiple times
        std::string msgStr;
        StringFormat(msgStr, format, args...);

        const LogInfo logMessage{ msgStr, file, line, channel, severity };

        // Start iterating on ourselves and stop when we're back on us (intrusive list is circular)
        IntrusiveListNode<LoggerBase>* loggerNode = this;
        do
        {
            LoggerBase* loggerOwner = loggerNode->Owner();
            if (MOE_ASSERT(loggerOwner != nullptr))
            {
                loggerOwner->ConsumeLogMessage(logMessage);
                loggerNode = loggerNode->Next();
            }
        } while (loggerNode != this);
    }
}
