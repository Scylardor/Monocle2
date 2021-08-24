
#include "catch.hpp"

#include "GameFramework/Service/TimeService/Stopwatch/Stopwatch.h"


TEST_CASE("Stopwatch", "[Time]")
{

	SECTION("Basic")
	{
		moe::Stopwatch watch;

		using namespace std::chrono_literals;

		watch.Restart();

		auto fakeNow = watch.GetStartTime() + 1s;
		moe::Stopwatch::SetNow(fakeNow);

		auto elapsedSecs = watch.Elapsed();
		REQUIRE(elapsedSecs == 1.f);

		auto elapsedMillis = watch.Elapsed<moe::ms>();
		REQUIRE(elapsedMillis == 1000);

		auto elapsedMicros = watch.Elapsed<moe::us>();
		REQUIRE(elapsedMicros == 1'000'000);

		auto elapsedNanos = watch.Elapsed<moe::ns>();
		REQUIRE(elapsedNanos == 1'000'000'000);

		elapsedSecs = watch.ElapsedSince<moe::sec>(fakeNow, fakeNow + 5s);
		REQUIRE(elapsedSecs == 5.f);
	}



	SECTION("States")
	{
		moe::Stopwatch watch;

		using namespace std::chrono_literals;

		watch.Restart();

		auto fakeNow = watch.GetStartTime() + 1s;
		moe::Stopwatch::SetNow(fakeNow);

		watch.Pause();
		auto elapsedSecs = watch.Elapsed();
		REQUIRE(elapsedSecs == 0.f);
		auto elapsedMillis = watch.Elapsed<moe::ms>();
		REQUIRE(elapsedMillis == 0);
		auto elapsedMicros = watch.Elapsed<moe::us>();
		REQUIRE(elapsedMicros == 0);
		auto elapsedNanos = watch.Elapsed<moe::ns>();
		REQUIRE(elapsedNanos == 0);

		watch.Unpause();
		elapsedSecs = watch.Elapsed();
		REQUIRE(elapsedSecs == 1.f);
		elapsedMillis = watch.Elapsed<moe::ms>();
		REQUIRE(elapsedMillis == 1000);
		elapsedMicros = watch.Elapsed<moe::us>();
		REQUIRE(elapsedMicros == 1'000'000);
		elapsedNanos = watch.Elapsed<moe::ns>();
		REQUIRE(elapsedNanos == 1'000'000'000);

		watch.Freeze();

		fakeNow += 5s;
		moe::Stopwatch::SetNow(fakeNow);

		elapsedSecs = watch.Elapsed();
		REQUIRE(elapsedSecs == 1.f);
		elapsedMillis = watch.Elapsed<moe::ms>();
		REQUIRE(elapsedMillis == 1000);
		elapsedMicros = watch.Elapsed<moe::us>();
		REQUIRE(elapsedMicros == 1'000'000);
		elapsedNanos = watch.Elapsed<moe::ns>();
		REQUIRE(elapsedNanos == 1'000'000'000);

		watch.Unfreeze();

		elapsedSecs = watch.Elapsed();
		REQUIRE(elapsedSecs == 6.f);
		elapsedMillis = watch.Elapsed<moe::ms>();
		REQUIRE(elapsedMillis == 6000);
		elapsedMicros = watch.Elapsed<moe::us>();
		REQUIRE(elapsedMicros == 6'000'000);
		elapsedNanos = watch.Elapsed<moe::ns>();
		REQUIRE(elapsedNanos == 6'000'000'000);

	}

}

