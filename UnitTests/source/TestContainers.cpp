#include <fstream>
#include <memory>


#include "catch.hpp"


// At the moment, tell Monocle we use std::string for our tests
#ifndef MOE_STD_SUPPORT
#define MOE_STD_SUPPORT
#endif


#include "Core/Containers/Vector/Vector.h"
#include "Core/Containers/HashMap/HashMap.h"
#include "Core/Containers/Array/Array.h"

struct NotCopyable
{
	NotCopyable() = default;

	NotCopyable(int val)
	{
		m_value = std::make_unique<int>(val);
	}
	NotCopyable(const NotCopyable& other) = delete;
	NotCopyable& operator=(const NotCopyable& rhs) = delete;
	NotCopyable(NotCopyable&& nc)
	{
		m_value = std::move(nc.m_value);
	}

	std::unique_ptr<int> m_value = 0;
};


class TestObject
{
public:

	TestObject()
	{
		s_defaultCtorCount++;
	}

	TestObject(const TestObject& other) :
		m_anInteger(other.m_anInteger)
	{}

	TestObject(int itg) : m_anInteger(itg)
	{
		s_valueCtorCount++;
	}

	TestObject(TestObject&& other)
	{
		s_moveCtorCount++;

		m_anInteger = other.m_anInteger;
		other.m_anInteger = 0;
	}

	TestObject& operator=(const TestObject& rhs)
	{
		m_anInteger = rhs.m_anInteger;
		return *this;
	}

	bool operator==(int itg) const
	{
		return itg == m_anInteger;
	}

	bool operator==(const TestObject& other) const
	{
		return other.m_anInteger == m_anInteger;
	}

	static int s_defaultCtorCount;
	static int s_valueCtorCount;
	static int s_moveCtorCount;

private:
	int m_anInteger = 0;
};


int	TestObject::s_defaultCtorCount = 0;
int	TestObject::s_valueCtorCount = 0;
int	TestObject::s_moveCtorCount = 0;

TEST_CASE("Containers", "[Core]")
{


    SECTION("Vector")
    {
		// Constructors + basic accessors
		bool eq = false;

		{
			moe::Vector<TestObject> test{ 1, 2, 3 };
			eq = test[0] == 1 && test[1] == 2 && test[2] == 3;
			CHECK(eq);
			CHECK(test.Begin() != test.End());
			CHECK(test.Size() == 3);
			CHECK(test.Capacity() >= test.Size());
		}

		moe::Vector<TestObject> testEmpty;
		CHECK(testEmpty.Size() == 0);
		CHECK(testEmpty.Begin() == testEmpty.End());
		CHECK(testEmpty.Capacity() >= testEmpty.Size());

		moe::Vector<TestObject> testN(5);
		CHECK(testN.Size() == 5);

		eq = (testN == moe::Vector<TestObject>{0, 0, 0, 0, 0});
		CHECK(eq);

		eq = (testN != moe::Vector<TestObject>{1, 0, 0, 0, 0});
		CHECK(eq);

		eq = (testN != moe::Vector<TestObject>{0, 0, 0, 0, 1});
		CHECK(eq);

		eq = (testN != moe::Vector<TestObject>{0, 0, 1, 0, 0});
		CHECK(eq);

		// Push back
		moe::Vector<TestObject> test;
		for (int iter = 0; iter != 10; iter++)
		{
			test.PushBack(iter);
			CHECK(test.Size() == iter + 1);
			CHECK(test.Begin() != test.End());
			CHECK(*test.Begin() == test[0]);
			CHECK(*(test.End() - 1) == test[test.Size() - 1]);
			CHECK(test.Capacity() >= test.Size());
		}

		int oldMoveCtorCount = TestObject::s_moveCtorCount;
		test.PushBack(TestObject());
		CHECK(TestObject::s_moveCtorCount == oldMoveCtorCount + 1);

		// Push back with non copyables
		moe::Vector<NotCopyable> testNC;
		testNC.PushBack(NotCopyable());

		// Emplace (should call ctor only once)
		int oldDefCtorCount = TestObject::s_defaultCtorCount;
		int oldValCtorCount = TestObject::s_valueCtorCount;
		oldMoveCtorCount = TestObject::s_moveCtorCount;
		size_t oldsize = test.Size();
		test.Emplace(test.End(), 5454);
		CHECK(test.Size() == oldsize + 1);
		CHECK(test[test.Size()-1] == 5454);
		CHECK(TestObject::s_defaultCtorCount == oldDefCtorCount);
		CHECK(TestObject::s_moveCtorCount == oldMoveCtorCount);
		CHECK(TestObject::s_valueCtorCount == oldValCtorCount + 1);

		oldValCtorCount = TestObject::s_valueCtorCount;
		oldsize = test.Size();
		test.EmplaceBack(5555);
		CHECK(test.Size() == oldsize + 1);
		CHECK(test[test.Size() - 1] == 5555);
		CHECK(TestObject::s_defaultCtorCount == oldDefCtorCount);
		CHECK(TestObject::s_moveCtorCount == oldMoveCtorCount);
		CHECK(TestObject::s_valueCtorCount == oldValCtorCount + 1);

		// Clear
		testEmpty.Clear();
		test.Clear();
		CHECK(testEmpty.Size() == 0);
		CHECK(testEmpty.Capacity() == 0);
		CHECK(testEmpty.Begin() == testEmpty.End());
		CHECK(test.Size() == 0);
		CHECK(test.Begin() == test.End());

		// Erase
		test = { 0, 1, 2, 3, 4, 5 };
		moe::Vector<TestObject>::Iterator it = test.Erase(test.Begin());
		CHECK(test.Size() == 5);
		CHECK(test.Begin() == it);
		CHECK(*test.Begin() == 1);
		eq = (test == moe::Vector<TestObject>{ 1, 2, 3, 4, 5 });
		CHECK(eq);

		it = test.Erase(test.End() - 1);
		CHECK(test.Size() == 4);
		CHECK(test.End() == it);
		CHECK(*(test.End() - 1) == 4);
		eq = (test == moe::Vector<TestObject>{ 1, 2, 3, 4 });
		CHECK(eq);

		test = { 0, 1, 2, 3, 4, 5 };
		it = test.Erase(test.Begin(), test.Begin() + 2);
		CHECK(test.Size() == 4);
		CHECK(test.Begin() == it);
		CHECK(*(test.Begin()) == 2);
		eq = (test == moe::Vector<TestObject>{ 2, 3, 4, 5 });
		CHECK(eq);

		it = test.Erase(test.Begin() + 1, test.Begin() + 3);
		CHECK(test.Size() == 2);
		CHECK(test.End()-1 == it);
		CHECK(*(test.Begin()) == 2);
		CHECK(*(test.End()-1) == 5);
		eq = (test == moe::Vector<TestObject>{ 2, 5 });
		CHECK(eq);

		// Erase with last swap
		test = { 0, 1, 2, 3, 4, 5 };
		auto objIt = test.EraseBySwapAt(3);
		CHECK(test.Size() == 5);
		CHECK(*objIt == 5);
		eq = (test == moe::Vector<TestObject>{ 0, 1, 2, 5, 4 });
		CHECK(eq);

		objIt = test.EraseBySwap(test.Begin());
		CHECK(*objIt == 4);
		CHECK(test.Size() == 4);
		eq = (test == moe::Vector<TestObject>{ 4, 1, 2, 5 });
		CHECK(eq);

		eq = (test == moe::Vector<TestObject>{ 4, 1, 2 });
		objIt = test.EraseBySwap(test.End() - 1); // erase last swap the last element should imply no swap at all
		CHECK(test.Size() == 3);
		eq = (test == moe::Vector<TestObject>{ 4, 1, 2 });
		CHECK(eq);

		// Insert
		test.Insert(test.End(), 5);
		eq = (test == moe::Vector<TestObject>{ 4, 1, 2, 5 });
		CHECK(eq);

		test.Insert(test.Begin() + test.Size() / 2, 5, 42);
		eq = (test == moe::Vector<TestObject>{ 4, 1, 42, 42, 42, 42, 42, 2, 5 });
		CHECK(eq);

		test.Insert(test.End(), { 69, 70, 71 });
		eq = (test == moe::Vector<TestObject>{ 4, 1, 42, 42, 42, 42, 42, 2, 5, 69, 70, 71 });
		CHECK(eq);

		// Resize, Capacity, Reserve
		test.Clear();

		test = moe::Vector<TestObject>{ 4, 1, 2, 5 };
		size_t oldCap = test.Capacity();
		test.Resize(2);
		eq = (test == moe::Vector<TestObject>{ 4, 1 });
		CHECK(eq);
		CHECK(test.Capacity() == oldCap);
		test.Resize(0); // resize 0 should not change the capacity
		test.Reserve(42);
		CHECK(test.Capacity() == 42);


		// Misc
		test = moe::Vector<TestObject>{ 4, 1, 2, 5 };
		eq = (test.Data() == &test[0]);
		CHECK(eq);

		// Range for
		for (auto item : test)
		{
			(void)item;
		}
    }


	SECTION("HashMap")
	{
		moe::HashMap<int, TestObject> test;
		CHECK(test.Size() == 0);

		// Insert
		auto insertResult = test.Insert(std::make_pair(42, 1337));
		REQUIRE(insertResult.second == true);
		REQUIRE(insertResult.first->first == 42); // key
		REQUIRE(insertResult.first->second == 1337); // value
		CHECK(test.Size() == 1);

		test.Insert({ {0, 233}, {222, 2123}, {3994, 2811} });
		CHECK(test.Size() == 4);

		// Emplace
		test.Emplace(8658, 3265);
		CHECK(test.Size() == 5);
		CHECK(test[8658] == 3265);


		// Find

		moe::HashMap<int, TestObject>::ConstIterator hmIt;
		bool eq;

		hmIt = test.Find(0);
		eq = (hmIt != test.End() && hmIt->second == 233);
		CHECK(eq);
		hmIt = test.Find(222);
		eq = (hmIt != test.End() && hmIt->second == 2123);
		CHECK(eq);
		hmIt = test.Find(3994);
		eq = (hmIt != test.End() && hmIt->second == 2811);
		CHECK(eq);
		CHECK(test.Find(419458) == test.End());

		moe::HashMap<int, TestObject> test2 = test;
		REQUIRE(test == test2);
		test2[3944] = 50000;
		REQUIRE(test != test2);


		// Erase
		test2.Erase(test2.Begin());
		CHECK(test2.Size() == 5);

		test2.Erase(3944);
		CHECK(test2.Size() == 4);

		// Clear
		test2.Clear();
		CHECK(test2.Size() == 0);

		// Misc
		// Range for
		for (auto item : test)
		{
			(void)item;
		}
	}


	SECTION("Array")
	{
		// default initialization (local = automatic storage):
		moe::Array<int, 3> first;              // uninitialized:    {?,?,?}
		CHECK(first.Size() == 3);

		// Size
		// initializer-list initializations:
		moe::Array<int, 3> second = { 10,20 };   // initialized as:   {10,20,0}
		CHECK(first.Size() == 3);
		bool eq;
		eq = (second[0] == 10 && second[1] == 20);
		CHECK(eq);

		moe::Array<int, 3> third = { 1,2,3 };    // initialized as:   {1,2,3}
		eq = (third[0] == 1 && third[1] == 2 && third[2] == 3);
		CHECK(eq);

		// copy initialization:
		moe::Array<int, 3> fourth = third;     // copy:             {1,2,3}
		eq = (fourth[0] == 1 && fourth[1] == 2 && fourth[2] == 3);
		CHECK(eq);

		// Fill
		fourth.Fill(2);
		eq = (fourth == moe::Array<int, 3>{2, 2, 2});
		CHECK(eq);

		// Misc
		// Range for
		for (auto item : fourth)
		{
			(void)item;
		}
	}


 }

