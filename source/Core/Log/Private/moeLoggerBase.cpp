#include "Core/Log/moeLoggerBase.h"

namespace
{
    // The dummy logger is just used by the log chain to dispatch messages to linked loggers.
    class DummyLogger : public moe::LoggerBase
    {

    public:
        virtual void    ConsumeLogMessage(const moe::LogInfo& logInfo) override { (void)logInfo; }

    };
}
namespace moe
{


    LoggerBase& GetLogChainSingleton()
    {
        static DummyLogger singleton; // The dummy logger represents the "head" of the chain, doesn't log anything itself

        return singleton;

    }
}
