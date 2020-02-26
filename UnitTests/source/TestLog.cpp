#include <fstream>
#include "catch.hpp"

// At the moment, tell Monocle we use std::string for our tests
#ifndef MOE_STD_SUPPORT
#define MOE_STD_SUPPORT
#endif
#include "Core/Log/moeLog.h"

TEST_CASE("moeLog", "[Core]")
{
    typedef moe::StdLogger<moe::SeverityFilterPolicy, moe::DebuggerFormatPolicy, moe::CaptureWritePolicy> FilteredFormattedLogger;
    typedef moe::StdLogger<moe::SeverityFilterPolicy, moe::NoFormatPolicy, moe::OutStreamWritePolicy> FilteredStreamLogger;
    typedef moe::StdLogger<moe::NoFilterPolicy, moe::NoFormatPolicy, moe::CaptureWritePolicy> CaptureLogger;
    typedef moe::StdLogger<moe::NoFilterPolicy, moe::NoFormatPolicy, moe::FileWritePolicy> FileLogger;

    SECTION("Basic log")
    {
        CaptureLogger logger;

        MOE_LOGGER_INFO(logger, moe::ChanDefault, "%s %s %d!", "otter", "slide", 42);
        std::string captured = logger.GetWritePolicy().GetCapturedOutput();
        REQUIRE(captured == "otter slide 42!");

        logger.GetWritePolicy().Clear();
        MOE_LOGGER_INFO(logger, moe::ChanDefault, "Format %s without arguments");
        captured = logger.GetWritePolicy().GetCapturedOutput();
        REQUIRE(captured == "Format %s without arguments");
    }

    SECTION("File write log")
    {
        const char* filename = "MonocleTest.txt";

        // Open a file and truncate it to be sure it's empty...
        {
            std::ofstream logFile(filename, std::ofstream::trunc);
            CHECK(logFile.is_open());
        }

        // then use a file write logger to write in it...
        const char * logLine = "Test writing to file";
        FileLogger logger;
        logger.GetWritePolicy().OpenFile(filename);
        MOE_LOGGER_INFO(logger, moe::ChanDefault, logLine);
        logger.GetWritePolicy().Close();

        // test logging to closed file write policy doesn't crash
        MOE_LOGGER_INFO(logger, moe::ChanDefault, "This won't be logged");

        // then check there's what we wrote
        std::ifstream logStream(filename, std::ios::in | std::ios::binary);
        REQUIRE(logStream.is_open());
        logStream.seekg(0, std::ios::end);

        std::string logContents((std::size_t)logStream.tellg(), '\0');
        logStream.seekg(0, std::ios::beg);
        logStream.read(&logContents[0], logContents.size());

        REQUIRE(logContents == logLine);

        // Clean behind us
        logStream.close();
        CHECK(std::remove(filename) == 0);
    }

    SECTION("Logger chaining")
    {
        CaptureLogger logger1;
        CaptureLogger logger2;
        CaptureLogger logger3;

        // Order is important
        logger2.LinkTo(&logger1);
        logger3.LinkTo(&logger2);

        MOE_LOGGER_INFO(logger1, moe::ChanDefault, "Test chaining %d", 42);

        std::string expected = "Test chaining 42";
        std::string captured1 = logger1.GetWritePolicy().GetCapturedOutput();
        std::string captured2 = logger2.GetWritePolicy().GetCapturedOutput();
        std::string captured3 = logger3.GetWritePolicy().GetCapturedOutput();
        bool areEqual = captured1 == captured2 && captured2 == captured3 && captured3 == expected;
        REQUIRE(areEqual);

        // Test unlinking
        logger2.Unlink();

        // logger1 and logger3 only should now be linked
        MOE_LOGGER_INFO(logger1, moe::ChanDefault, "Test chaining 2");
        std::string expected2 = expected + "Test chaining 2";
        captured1 = logger1.GetWritePolicy().GetCapturedOutput();
        captured3 = logger3.GetWritePolicy().GetCapturedOutput();
        areEqual = captured1 == captured3 && captured3 == expected2;
        REQUIRE(areEqual);
        captured2 = logger2.GetWritePolicy().GetCapturedOutput();
        REQUIRE(captured2 == expected);

        // Now break the last link
        logger1.Unlink();
        MOE_LOGGER_INFO(logger1, moe::ChanDefault, "Test chaining 3");
        std::string expected3 = expected2 + "Test chaining 3";
        captured1 = logger1.GetWritePolicy().GetCapturedOutput();
        captured3 = logger3.GetWritePolicy().GetCapturedOutput();
        REQUIRE(captured1 == expected3);
        REQUIRE(captured3 == expected2);

        // Unlink without link should do nothing
        logger1.Unlink();
        MOE_LOGGER_INFO(logger1, moe::ChanDefault, "Test chaining 4");
        expected = expected3 + "Test chaining 4";
        captured1 = logger1.GetWritePolicy().GetCapturedOutput();
        REQUIRE(captured1 == expected);
    }

    SECTION("Test filtering and formatting")
    {
        FilteredFormattedLogger ffLogger;
        moe::CaptureWritePolicy& myCapturePolicy = ffLogger.GetWritePolicy();

        ffLogger.GetFilterPolicy().SetFilterSeverity(moe::SevError);

        // Inputting dummy values for line and file so we're independent of actual file/line values...
        // Below severity minimum
        MOE_LOG_TO_LOGGER(ffLogger, moe::ChanDefault, moe::SevInfo, "__FILE__", 42, "Not logged");
        std::string captured = myCapturePolicy.GetCapturedOutput();
        REQUIRE(captured.empty());

        // Still Below severity minimum
        MOE_LOG_TO_LOGGER(ffLogger, moe::ChanDefault, moe::SevWarning, "__FILE__", 42, "Not logged");
        captured = myCapturePolicy.GetCapturedOutput();
        REQUIRE(captured.empty());

        // Equal to severity minimum
        std::string expected = "__FILE__(42): [Default] (ERROR) First log";
        MOE_LOG_TO_LOGGER(ffLogger, moe::ChanDefault, moe::SevError, "__FILE__", 42, "First log");
        captured = myCapturePolicy.GetCapturedOutput();
        REQUIRE(captured == expected);

        // Above severity minimum, testing another channel
        expected += "__FILE__(42): [Debug] (FATAL) Second log"; // If we don't clear it, captured output should accumulate
        MOE_LOG_TO_LOGGER(ffLogger, moe::ChanDebug, moe::SevFatal, "__FILE__", 42, "Second log");
        captured = myCapturePolicy.GetCapturedOutput();
        REQUIRE(captured == expected);
    }

    SECTION("Default logger")
    {
        // Default logger should be null upon initialization
        moe::LoggerBase& logChain = moe::GetLogChainSingleton();

        // The "default" class of logging macros won't log if the log chain is empty
        // Just check it doesn't crash in any way
        MOE_INFO(moe::ChanDefault, "This won't be logged");

        // Now test by plugging a cout logger filtered on severity
        // and capture cout output to check its output
        std::stringstream captureStream;
        std::streambuf * origCoutBuffer = std::cout.rdbuf(captureStream.rdbuf());

        FilteredStreamLogger filteredLogger(moe::SevWarning, moe::NoFormatPolicy(), std::cout);
        filteredLogger.LinkTo(&logChain);

        MOE_INFO(moe::ChanDefault, "This won't pass the %s filter", "severity");

        std::string capturedText = captureStream.str();
        REQUIRE(capturedText.empty());

        // Now it should capture something
        MOE_WARNING(moe::ChanDefault, "This passes the %s filter", "severity");

        // Give std::cout its original buffer back ASAP
        std::cout.rdbuf(origCoutBuffer);

        // Note: output stream policy appends a \n to messages
        capturedText = captureStream.str();
        REQUIRE(capturedText == "This passes the severity filter\n");

        filteredLogger.Unlink();

        moe::StdLogger<moe::NoFilterPolicy, moe::DebuggerFormatPolicy, moe::IdeWritePolicy> ideLogger;
        ideLogger.LinkTo(&logChain);

        MOE_WARNING(moe::ChanDefault, "This passes the %s filter", "severity");
    }
 }

