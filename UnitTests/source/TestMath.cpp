
#include "catch.hpp"

#include "Math/Math.h"

#define _USE_MATH_DEFINES
#include <math.h>

// Taken from https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
// TODO: study it more and integrate into Math library
template<class T>
std::enable_if_t<!std::numeric_limits<T>::is_integer, bool>
AlmostEqual(T x, T y, int ulp)
{
	// the machine epsilon has to be scaled to the magnitude of the values used
	// and multiplied by the desired precision in ULPs (units in the last place)
	return std::fabs(x - y) <= std::numeric_limits<T>::epsilon() * std::fabs(x + y) * ulp
		// unless the result is subnormal
		|| std::fabs(x - y) < std::numeric_limits<T>::min();
}


TEST_CASE("Maths", "[Math]")
{

	SECTION("Vec2")
	{
		moe::Vec2 test1(42);
		REQUIRE((test1.x() == 42 && test1.y() == 42));

		moe::Vec2 test2(1, 0);
		REQUIRE((test2.x() == 1 && test2.y() == 0));

		moe::Vec2 test3(test2);
		REQUIRE(test3 == test2);

		test1[0] = 3;
		test1[1] = 4;
		REQUIRE((test1.x() == 3 && test1.y() == 4));

		moe::Vec2 zero = moe::Vec2::ZeroVector();
		REQUIRE(zero.Length() == 0);
		REQUIRE(zero == moe::Vec2(0));


		// TODO ...
//		float dist = zero.Distance(test2);

	//	float dot = test2.Dot(test3);
	}


	SECTION("Vec3")
	{
		moe::Vec3 test1(42);
		REQUIRE((test1.x() == 42 && test1.y() == 42));

		moe::Vec3 test2(1, 0, 2);
		REQUIRE((test2.x() == 1 && test2.y() == 0 && test2.z() == 2));

		moe::Vec3 test3(test2);
		REQUIRE(test3 == test2);

		test1[0] = 3;
		test1[1] = 4;
		test1[2] = 5;
		REQUIRE((test1.x() == 3 && test1.y() == 4 && test1.z() == 5));

		test1[0] = 0;
		test1[1] = 1;
		test1[2] = 0;
		REQUIRE(test1.Length() == 1);

		test1[1] = 2;
		REQUIRE(test1.Length() == 2);
		REQUIRE(test1.SquaredLength() == 4);

		moe::Vec3 zero = moe::Vec3::ZeroVector();
		REQUIRE(zero.Length() == 0);
		REQUIRE(zero == moe::Vec3(0));
	}


	SECTION("Vec4")
	{
		moe::Vec4 test1(42);
		REQUIRE((test1.x() == 42 && test1.y() == 42));

		moe::Vec4 test2(1, 0, 2, 3);
		REQUIRE((test2.x() == 1 && test2.y() == 0 && test2.z() == 2 && test2.w() == 3));

		moe::Vec4 test3(test2);
		REQUIRE(test3 == test2);

		test1[0] = 3;
		test1[1] = 4;
		test1[2] = 5;
		test1[3] = 6;
		REQUIRE((test1.x() == 3 && test1.y() == 4 && test1.z() == 5 && test1.w() == 6));

		test1[0] = 0;
		test1[1] = 0;
		test1[2] = 1;
		test1[3] = 0;
		REQUIRE(test1.Length() == 1);

		test1[2] = 2;
		REQUIRE(test1.Length() == 2);
		REQUIRE(test1.SquaredLength() == 4);

		moe::Vec4 zero = moe::Vec4::ZeroVector();
		REQUIRE(zero.Length() == 0);
		REQUIRE(zero == moe::Vec4(0));
	}


	SECTION("Matrix")
	{
		moe::Mat4 zeroMatrix(0);

		// Init from float array
		float seize[16] = { 0 };
		moe::Mat4 testmatarray(seize);

		REQUIRE(testmatarray == zeroMatrix);

		moe::Mat4 identity1(1);
		moe::Mat4 identity2(moe::Mat4::Identity());
		REQUIRE(identity1 == identity2);

		moe::Vec4& moematcol = identity1[0];
		moematcol[0] = 42.f;
		REQUIRE(identity1[0][0] == 42.f);


		moe::Vec4 testTransf(1, 2, 3, 4);
		moe::Vec4 testTransf2(1, 2, 3, 4);

		testTransf2 = identity2 * testTransf2;
		REQUIRE(testTransf2 == testTransf);

		moe::Mat3x4	testmat34 = moe::Mat3x4::Identity();
		testTransf2 = testmat34 * testTransf2;
		REQUIRE(testTransf2.xyz() == testTransf.xyz());

		moe::Mat4 identityTranspo = identity2.GetTransposed();
		REQUIRE(identity2 == identityTranspo);

		// TODO ...
		moe::Mat4 testproj = moe::Mat4::Perspective(45_degf, 1.f, 1.f, 1000.f);

		moe::Mat4 inverse = testproj.GetInverse();

		float testfloats[16] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
		moe::Mat4 testmat2(testfloats);
		identityTranspo /= testmat2;

		moe::Mat4 testtransfos = moe::Mat4::Scaling({ 1, 2, 3 });
		testtransfos = moe::Mat4::Scaling( 1, 2, 3 );
		testtransfos.Scale(1, 2, 3);
		testtransfos.Scale({1, 2, 3});

		testtransfos = moe::Mat4::Rotation(45_degf, 1, 0, 0);
		testtransfos = moe::Mat4::Rotation(45_degf, { 1, 0, 0 });
		testtransfos.Rotate(45_degf, 1, 2, 3);
		testtransfos.Rotate(45_degf, { 1, 2, 3 });

		testtransfos = moe::Mat4::Translation({ 1, 2, 3 });
		testtransfos = moe::Mat4::Translation(1, 2, 3);
		testtransfos.Translate(1, 2, 3);
		testtransfos.Translate({ 1, 2, 3 });

	}


	SECTION("Angles")
	{
		moe::Degs_f degs = 90_degf;
		CHECK(degs == 90.f);

		moe::Rads_f rads = 0.5_radf;
		CHECK(rads == 0.5f);

		rads = degs; // automatic degrees->radian conversion
		CHECK(AlmostEqual((float)rads, (float)M_PI_2, 2));

		degs = rads;
		CHECK(degs == 90.f);

	}

 }

