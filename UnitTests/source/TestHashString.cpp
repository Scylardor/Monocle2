#include "catch.hpp"

// At the moment, tell Monocle we use std::string for our tests
#ifndef MOE_STD_SUPPORT
#define MOE_STD_SUPPORT
#endif

#include "Core/HashString/HashString.h"

/*
	This is to verify string hashes produce expected results
*/
TEST_CASE("HashString", "[Core]")
{
    SECTION("Basic")
    {
		//empty
		moe::HashString test;
		CHECK(test() == 0);
		CHECK(test.GetString() == "");

		// set without string
		test.Set(0x424242);
		CHECK(test.GetString() == "");
		CHECK(test() == 0x424242);

		// clear
		test.Clear();
		CHECK(test() == 0);
		CHECK(test.GetString() == "");

		// other initializations
		test.Set(0x424242);
		moe::HashString test2 = test;
		CHECK(test.GetString() == "");
		CHECK(test() == 0x424242);
		CHECK(test2.GetString() == "");
		CHECK(test2() == 0x424242);

		moe::HashString test3(test);
		CHECK(test3.GetString() == "");
		CHECK(test3() == 0x424242);

		moe::HashString test4;
		test4 = test3;
		CHECK(test3.GetString() == "");
		CHECK(test3() == 0x424242);
		CHECK(test4.GetString() == "");
		CHECK(test4() == 0x424242);

		moe::HashString test5;
		test5 = std::move(test4);
		CHECK(test4.GetString() == "");
		CHECK(test4() == 0);
		CHECK(test5.GetString() == "");
		CHECK(test5() == 0x424242);

		moe::HashString test6(std::move(test5));
		CHECK(test5.GetString() == "");
		CHECK(test5() == 0);
		CHECK(test6.GetString() == "");
		CHECK(test6() == 0x424242);
    }

	// FNV-1a hasher should output same values everywhere
	SECTION("Hash value FNV-1a")
	{
		typedef moe::HashStringT<std::string, moe::FNV1aHashPolicy<std::string>> TestHashString;
		TestHashString test;
		TestHashString test2;
		TestHashString test3;

		test = "toto";
		CHECK(test() == 0xf6a4e63f5280494d);

		test2 = TestHashString("titi");
		CHECK(test2() == 0xec92d5697d2707f5);

		test3.SetString("tata");
		CHECK(test3() == 0xc6ea9a540e406295);

	}

	SECTION("Hash value std::hash")
	{
		// Since std implementations are compiler-specific, std::hash may yield different results based on what compiler we use.
		// And Hash functions are only required to produce the same result for the same input within a single execution of a program;
		// this allows salted hashes that prevent collision DoS attacks since C++14, but then we're unable to reliably test std::hash results.
		// cf. https://en.cppreference.com/w/cpp/utility/hash
	}

}
