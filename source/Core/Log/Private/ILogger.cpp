#include "Core/Log/ILogger.h"

namespace
{
    // The dummy logger is just used by the log chain to dispatch messages to linked loggers.
    class DummyLogger : public moe::ILogger
    {

    public:
        virtual void    ConsumeLogMessage(const moe::LogInfo& ) override {}

    };
}
namespace moe
{


    ILogger& GetLogChainSingleton()
    {
        static DummyLogger singleton; // The dummy logger represents the "head" of the chain, doesn't log anything itself

        return singleton;

    }
}
