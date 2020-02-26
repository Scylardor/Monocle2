#include <fstream>
#include "catch.hpp"

// At the moment, tell Monocle we use std::string for our tests
#ifndef MOE_STD_SUPPORT
#define MOE_STD_SUPPORT
#endif
#include "Core/Log/moeLog.h"


#include "GameFramework/FSM/FSM.h"


typedef moe::StdLogger<moe::NoFilterPolicy, moe::NoFormatPolicy, moe::CaptureWritePolicy> CaptureLogger;


class ATestFSM final : public moe::FSM
{
public:

	moe::FSM::TransitionID	Update()
	{
		return UpdateCurrentState();
	}


	CaptureLogger&	Logger() { return m_logger; }


private:
	CaptureLogger m_logger; // I use a capturable logger to check its output in order to make sure states are processed in the right order.

};


class ATestState final : public moe::FSM::IState
{

public:
	ATestState()
	{}

	ATestState(const std::string& pOne, const std::string& pTwo, const std::string& pThree) :
		m_one(pOne), m_two(pTwo), m_three(pThree)
	{}


	virtual void	OnStateEnter(moe::FSM& parentFsm, moe::FSM::StateID myId) override
	{
		ATestFSM& myFsm = static_cast<ATestFSM&>(parentFsm);
		MOE_LOGGER_INFO(myFsm.Logger(), moe::ChanDefault, "State %d enter: %s\n", myId, m_one);
	}

	virtual void	OnStateUpdate(moe::FSM& parentFsm, moe::FSM::StateID myId) override
	{
		ATestFSM& myFsm = static_cast<ATestFSM&>(parentFsm);
		MOE_LOGGER_INFO(myFsm.Logger(), moe::ChanDefault, "State %d update: %s\n", myId, m_two);
	}

	virtual void	OnStateExit(moe::FSM& parentFsm, moe::FSM::StateID myId) override
	{
		ATestFSM& myFsm = static_cast<ATestFSM&>(parentFsm);
		MOE_LOGGER_INFO(myFsm.Logger(), moe::ChanDefault, "State %d exit: %s\n", myId, m_three);
	}



private:
	std::string m_one = "one";
	std::string m_two = "two";
	std::string m_three = "three";
};



class IdleState final : public moe::FSM::IState
{
	virtual void	OnStateEnter(moe::FSM&, moe::FSM::StateID) override {}
	virtual void	OnStateUpdate(moe::FSM&, moe::FSM::StateID) override {}
	virtual void	OnStateExit(moe::FSM&, moe::FSM::StateID) override {}
};


class ATestTransition final : public moe::FSM::ITransition
{
public:
	ATestTransition(bool passes = true) :
		m_passes(passes)
	{}

	virtual bool	Passes(moe::FSM&, moe::FSM::IState&, moe::FSM::StateID) override
	{
		return m_passes;
	}

private:
	bool	m_passes = true;

};


TEST_CASE("FiniteStateMachine", "[GameFramework]")
{


    SECTION("Basic use")
    {
		moe::FSM myFsm;

		CHECK(!myFsm.IsRunning());
		CHECK(myFsm.Size() == 0);
		CHECK(!myFsm.HasState(0));
		CHECK(!myFsm.HasState(1));
		CHECK(!myFsm.HasState(2));
    }


    SECTION("Add/Remove states")
    {
		ATestFSM myFsm;
		CHECK(!myFsm.IsRunning());
		CHECK(myFsm.Size() == 0);

		// Test add state without parameters
		moe::FSM::StateID newStateID = myFsm.AddState<ATestState>();

		// Add should not make FSM run
		CHECK(!myFsm.IsRunning());

		CHECK(myFsm.Size() == 1);
		CHECK(newStateID == 0);


		// Test add state with variadic parameters
		newStateID = myFsm.AddState<ATestState>("uno", "dos", "tres");
		CHECK(!myFsm.IsRunning());

		CHECK(myFsm.Size() == 2);
		CHECK(newStateID == 1);

		CHECK(myFsm.HasState(0));
		CHECK(myFsm.HasState(newStateID));

		CHECK(!myFsm.HasState(2));

		newStateID = myFsm.AddState<IdleState>();
		CHECK(myFsm.Size() == 3);
		CHECK(newStateID == 2);
		CHECK(myFsm.HasState(0));
		CHECK(myFsm.HasState(1));
		CHECK(myFsm.HasState(newStateID));


		// Now we test state enter / update / exit
		myFsm.StartState(0);
		std::string captured = myFsm.Logger().GetWritePolicy().GetCapturedOutput();
		REQUIRE(captured == "State 0 enter: one\n");
		myFsm.Logger().GetWritePolicy().Clear();

		myFsm.StopFSM();
		captured = myFsm.Logger().GetWritePolicy().GetCapturedOutput();
		REQUIRE(captured == "State 0 exit: three\n");
		myFsm.Logger().GetWritePolicy().Clear();

		myFsm.StartState(0);
		captured = myFsm.Logger().GetWritePolicy().GetCapturedOutput();
		REQUIRE(captured == "State 0 enter: one\n");
		myFsm.Logger().GetWritePolicy().Clear();

		myFsm.Update();
		captured = myFsm.Logger().GetWritePolicy().GetCapturedOutput();
		REQUIRE(captured == "State 0 update: two\n");
		myFsm.Logger().GetWritePolicy().Clear();

		myFsm.StartState(1);
		captured = myFsm.Logger().GetWritePolicy().GetCapturedOutput();
		REQUIRE(captured == "State 0 exit: three\nState 1 enter: uno\n");
		myFsm.Logger().GetWritePolicy().Clear();

		myFsm.Update();
		captured = myFsm.Logger().GetWritePolicy().GetCapturedOutput();
		REQUIRE(captured == "State 1 update: dos\n");
		myFsm.Logger().GetWritePolicy().Clear();

		myFsm.StopFSM();
		captured = myFsm.Logger().GetWritePolicy().GetCapturedOutput();
		REQUIRE(captured == "State 1 exit: tres\n");
		myFsm.Logger().GetWritePolicy().Clear();


		// Now we remove

		myFsm.RemoveState(2);
		CHECK(myFsm.Size() == 2);
		CHECK(!myFsm.HasState(2));

		myFsm.RemoveState(1);
		CHECK(myFsm.Size() == 1);
		CHECK(!myFsm.HasState(1));

		myFsm.RemoveState(0);
		CHECK(myFsm.Size() == 0);
		CHECK(!myFsm.HasState(0));
    }


   SECTION("Add/Remove transitions")
   {
	moe::FSM myFsm;

	moe::FSM::StateID firstStateID = myFsm.AddState<ATestState>();
	moe::FSM::StateID secondStateID = myFsm.AddState<ATestState>();

	moe::FSM::TransitionID newTransID = myFsm.AddTransition<ATestTransition>(firstStateID, secondStateID);
	CHECK(newTransID == 0);

	moe::FSM::TransitionNumber transNbr = myFsm.GetStateNumberOfTransitions(firstStateID);
	CHECK(transNbr == 1);
	transNbr = myFsm.GetStateNumberOfTransitions(secondStateID);
	CHECK(transNbr == 0);

	newTransID = myFsm.AddTransition<ATestTransition>(firstStateID, secondStateID);
	CHECK(newTransID == 1);
	transNbr = myFsm.GetStateNumberOfTransitions(firstStateID);
	CHECK(transNbr == 2);

	myFsm.RemoveStateTransition(firstStateID, newTransID);
	transNbr = myFsm.GetStateNumberOfTransitions(firstStateID);
	CHECK(transNbr == 1);
	myFsm.RemoveStateTransition(firstStateID, 0);
	transNbr = myFsm.GetStateNumberOfTransitions(firstStateID);
	CHECK(transNbr == 0);

	myFsm.AddTransition<ATestTransition>(firstStateID, secondStateID);
	myFsm.AddTransition<ATestTransition>(firstStateID, secondStateID);
	transNbr = myFsm.GetStateNumberOfTransitions(firstStateID);
	CHECK(transNbr == 2);

	// Removing a destination state should remove all the transitions pointing to it in other states
	myFsm.RemoveState(secondStateID);
	transNbr = myFsm.GetStateNumberOfTransitions(firstStateID);
	CHECK(transNbr == 0);

	secondStateID = myFsm.AddState<ATestState>();
	myFsm.AddTransition<ATestTransition>(firstStateID, secondStateID);
	myFsm.AddTransition<ATestTransition>(firstStateID, secondStateID);

	transNbr = myFsm.GetStateNumberOfTransitions(firstStateID);
	CHECK(transNbr == 2);
	transNbr = myFsm.RemoveStateTransition(firstStateID, 0);
	CHECK(transNbr == 1);
	// Transition numbers are just indices in a table : when we remove one, the same transition can now have a different transition ID
	transNbr = myFsm.RemoveStateTransition(firstStateID, 0);
	CHECK(transNbr == 0);
   }


   SECTION("Transitioning from state to state")
   {
	   ATestFSM myFsm;

	   moe::FSM::StateID firstStateID = myFsm.AddState<ATestState>();
	   moe::FSM::StateID secondStateID = myFsm.AddState<ATestState>("uno", "dos", "tres");

	   // A transition that never passes
	   myFsm.AddTransition<ATestTransition>(firstStateID, secondStateID, false);

	   myFsm.StartState(0);
	   moe::FSM::TransitionID passedTransition = myFsm.Update();
	   CHECK(passedTransition == moe::FSM::s_InvalidTransitionID); // didn't pass

	   // A transition that always passes
	   myFsm.AddTransition<ATestTransition>(firstStateID, secondStateID, true);
	   passedTransition = myFsm.Update();
	   CHECK(passedTransition == 1); // should pass

	   std::string captured = myFsm.Logger().GetWritePolicy().GetCapturedOutput();
	   REQUIRE(captured == "State 0 enter: one\nState 0 update: two\nState 0 update: two\nState 0 exit: three\nState 1 enter: uno\n");
	   myFsm.Logger().GetWritePolicy().Clear();

   }

 }

