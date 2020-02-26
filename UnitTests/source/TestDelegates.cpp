
#include "catch.hpp"


// At the moment, tell Monocle we use std::string for our tests
#ifndef MOE_STD_SUPPORT
#define MOE_STD_SUPPORT
#endif

#include "Core/Delegates/Delegate.h"
#include "Core/Delegates/Event.h"

static int test = 0;

void testfunc()
{
	test++;
}

void othertestfunc()
{
	test--;
}

struct UserStruct
{
	void member()
	{
		test++;
	}


};


TEST_CASE("Delegates", "[Core]")
{
	SECTION("Delegate")
	{
		test = 0;

		moe::Delegate<void()> del;
		CHECK(!del.IsSet());
		del.Set<&testfunc>();
		CHECK(del.IsSet());
		del();
		CHECK(test == 1);

		// Set by constructor
		moe::Delegate<void()> del2 = &testfunc;
		CHECK(del2.IsSet());
		del2();
		CHECK(test == 2);

		// Test member function
		UserStruct testUs;
		moe::Delegate<void()> del3(&testUs, &UserStruct::member);
		CHECK(del3.IsSet());
		del3();
		CHECK(test == 3);

		// Reassign
		del2 = &othertestfunc;
		CHECK(del2.IsSet());
		del2();
		CHECK(test == 2);
	}

	SECTION("Event")
	{
		test = 0;

		moe::Event<void()>	testEvent;
		CHECK(testEvent.GetNextID() == 0);

		moe::EventDelegateID dlgId = testEvent.Add<&testfunc>();
		CHECK(testEvent.GetNextID() == 1);
		CHECK(dlgId == 0);

		CHECK(test == 0);
		testEvent.Broadcast();
		CHECK(test == 1);

		// The same function can be bound multiple times
		moe::EventDelegateID dlgId2 = testEvent.Add<&testfunc>();
		CHECK(dlgId2 == 1);

		testEvent.Broadcast();
		CHECK(test == 3);

		// Remove
		bool removeOk = testEvent.Remove(dlgId);
		CHECK(removeOk);
		testEvent.Broadcast();
		CHECK(test == 4);
		moe::EventDelegateID dlgId3 = testEvent.Add<&testfunc>();
		CHECK(dlgId3 == 2);
		testEvent.Broadcast();
		CHECK(test == 6);


		// Test member function
		UserStruct testUs;

		moe::EventDelegateID dlgId4 = testEvent.Add<UserStruct, &UserStruct::member>(&testUs);
		CHECK(dlgId4 == 3);

		testEvent.Broadcast();
		CHECK(test == 9);

		// Test AddDelegate
		moe::Delegate<void()> del;
		del.Set<&testfunc>();
		moe::EventDelegateID dlgId5 = testEvent.AddDelegate(del);
		CHECK(dlgId5 == 4);

		testEvent.Broadcast();
		CHECK(test == 13);

		removeOk = testEvent.Remove(dlgId5);
		CHECK(removeOk);

		del.Set<UserStruct, &UserStruct::member>(&testUs);
		dlgId5 = testEvent.AddDelegate(del);
		CHECK(dlgId5 == 5);

		testEvent.Broadcast();
		CHECK(test == 17);

		// Test move AddDelegate
		removeOk = testEvent.Remove(dlgId5);
		CHECK(removeOk);

		dlgId5 = testEvent.AddDelegate(std::move(del));
		CHECK(!del.IsSet());
		CHECK(dlgId5 == 6);

		testEvent.Broadcast();
		CHECK(test == 21);

	}

 }

