#include "catch.hpp"

// At the moment, tell Monocle we use std::string for our tests
#ifndef MOE_STD_SUPPORT
#define MOE_STD_SUPPORT
#endif
#include "Core/StringFormat/moeStringFormat.h"

// All tests should verify both char and wchar_t support.
TEST_CASE("StringFormat", "[Core]")
{
    std::string     testString;
    std::wstring    wideTestString;

    // A normal string format
    SECTION("Normal string format")
    {
        moe::StringFormat(testString, "The answer is %d, 42 is char %c, float 42 is %.2f, and %s is cool", 42, 42, 42.f, "Catch");
        REQUIRE(testString == "The answer is 42, 42 is char *, float 42 is 42.00, and Catch is cool");

        moe::StringFormat(wideTestString, L"Answer is %d and '%ls' means french in French", 42, L"français");
        REQUIRE(wideTestString == L"Answer is 42 and 'français' means french in French");
    }

    SECTION("Format string without arguments")
    {
        const char* format = "%s %f %d %i %lp";
        moe::StringFormat(testString, format);
        REQUIRE(testString == format);

        const wchar_t* wideFormat = L"%s %f %d %i %lp";
        moe::StringFormat(wideTestString, wideFormat);
        REQUIRE(wideTestString == wideFormat);
    }

    SECTION("Custom format arguments")
    {
        // actually std::string is a case of custom format arg provided by Monocle when MOE_STD_SUPPORT is defined
        std::string catchStr("Catch");

        moe::StringFormat(testString, "The answer is %d and %s is cool", 42, catchStr);
        REQUIRE(testString == "The answer is 42 and Catch is cool");

        std::wstring wideCatchStr(L"Câtch");
        moe::StringFormat(wideTestString, L"The answer is %d and %ls is cool", 42, wideCatchStr);
        REQUIRE(wideTestString == L"The answer is 42 and Câtch is cool");

        // TODO: add an ad hoc custom arg for testing purpose ? (one not provided by the engine)
    }

    // TODO: add a test to verify format
}
