//// Monocle Game Engine source files - Alexandre Baron
//
//#pragma once
//
//#include "Core/Preprocessor/moeDLLVisibility.h"
#include "Core/Containers/Vector/Vector.h"
#include "Core/Containers/HashMap/HashMap.h"
#include "Core/Containers/Array/Array.h"

#include <cstdint>
#include <memory>

namespace moe
{
	class FSM
	{
	public:

		typedef std::size_t	StateID;
		typedef StateID	StatesNumber;

		typedef std::size_t	TransitionID;
		typedef TransitionID	TransitionNumber;

		static const StatesNumber s_UninitializedState = 0xffffffff;
		static const TransitionID s_InvalidTransitionID = 0xffffffff;


		class IState
		{
		public:

			virtual void	OnStateEnter(FSM& owner, StateID thisStateID) = 0;
			virtual void	OnStateUpdate(FSM& owner, StateID thisStateID) = 0;
			virtual void	OnStateExit(FSM& owner, StateID thisStateID) = 0;
		};


		class ITransition
		{
		public:

			virtual bool	Passes(FSM& fsm, IState& stateFrom, StateID fromID) = 0;
		};


	protected:

		struct TransitionDataTest
		{
			TransitionDataTest(std::unique_ptr<ITransition>&& ptr, StateID dest) :
				m_transition(std::move(ptr)), m_destination(dest) {}

			std::unique_ptr<ITransition>	m_transition = nullptr;
			StateID							m_destination;
		};


		struct StateDataTest
		{
			typedef	moe::Vector<TransitionDataTest>	StateTransitionsData;

			StateTransitionsData	m_transitionData;
			std::unique_ptr<IState>	m_state = nullptr;
		};


		StateDataTest&				ModifyStateData(moe::FSM::StateID stateID)
		{
			FSMStatesData::Iterator stateDataIt = m_stateData.Find(stateID);
			MOE_DEBUG_ASSERT(stateDataIt != m_stateData.End());

			StateDataTest& stateData = stateDataIt->second;
			MOE_DEBUG_ASSERT(stateData.m_state != nullptr);

			return stateData;
		}


		const StateDataTest&		GetStateData(moe::FSM::StateID stateID) const
		{
			FSMStatesData::ConstIterator stateDataIt = m_stateData.Find(stateID);
			MOE_DEBUG_ASSERT(stateDataIt != m_stateData.End());

			const StateDataTest& stateData = stateDataIt->second;
			MOE_DEBUG_ASSERT(stateData.m_state != nullptr);

			return stateData;
		}


		TransitionID	UpdateCurrentState()
		{
			MOE_DEBUG_ASSERT(IsRunning());

			TransitionID passedTransitionID = FSM::s_InvalidTransitionID;

			StateDataTest& currentStateData = ModifyStateData(m_currentStateID);
			currentStateData.m_state->OnStateUpdate(*this, m_currentStateID);

			for (TransitionID iTransition = 0; iTransition < currentStateData.m_transitionData.Size(); ++iTransition)
			{
				TransitionDataTest& transData = currentStateData.m_transitionData[iTransition];
				MOE_DEBUG_ASSERT(transData.m_transition != nullptr);

				if (transData.m_transition->Passes(*this, *currentStateData.m_state, m_currentStateID))
				{
					passedTransitionID = iTransition;
					StateDataTest& nextStateData = ModifyStateData(transData.m_destination);

					TransitionFromStateToState(m_currentStateID, *currentStateData.m_state, transData.m_destination, *nextStateData.m_state);
					break;
				}
			}

			return passedTransitionID;
		}


	private:
		typedef	HashMap<StateID, StateDataTest>	FSMStatesData;

		FSMStatesData		m_stateData;
		StateID				m_currentStateID = s_UninitializedState;
		StatesNumber		m_stateNbr = 0; // Current number of states in the FSM
		TransitionNumber	m_transitionNbr = 0; // Current number of transitions in the FSM


		void	TransitionFromStateToState(StateID fromID, IState& fromState, StateID toID, IState& toState)
		{
			fromState.OnStateExit(*this, fromID);
			toState.OnStateEnter(*this, toID);
			m_currentStateID = toID;
		}


	public:

		MOE_DLL_API bool	HasState(StateID id) const;


		MOE_DLL_API void 	StartState(moe::FSM::StateID id);


		/*	A state removal is somewhat heavy since it removes the state from the states hash map but also parses other states' transition data
			to eliminate all transitions pointing to the removed state. Use carefully. */
		MOE_DLL_API void 	RemoveState(moe::FSM::StateID removedId);


		MOE_DLL_API TransitionNumber 	RemoveStateTransition(FSM::StateID stateId, FSM::TransitionID transId);


		MOE_DLL_API TransitionNumber 	GetStateNumberOfTransitions(moe::FSM::StateID id) const;


		MOE_DLL_API void 	StopFSM();


		MOE_DLL_API void 	Clear();


		bool	IsRunning() const
		{
			return (m_currentStateID != s_UninitializedState);
		}


		StatesNumber 	Size() const
		{
			return m_stateData.Size();
		}



		template <class StateType, typename... Args>
		moe::FSM::StateID	AddState(Args... ctorParams)
		{
			static_assert(std::is_base_of<IState, StateType>::value, "Trying to add a state that doesn't implement IState interface");

			// TODO: replace this standard unique pointer...
			StateDataTest newStateData;
			newStateData.m_state = std::make_unique<StateType>(std::forward<Args>(ctorParams)...);

			MOE_DEBUG_ASSERT(m_stateNbr != s_UninitializedState);
			// TODO: encapsulate those std::pair...
			const std::pair<FSMStatesData::Iterator, bool> insertResult = m_stateData.Insert({ m_stateNbr, std::move(newStateData) });
			MOE_DEBUG_ASSERT(insertResult.second == true);

			m_stateNbr++;
			MOE_DEBUG_ASSERT(m_stateNbr != s_UninitializedState);

			return (m_stateNbr - 1);
		}


		template <class TransitionType, typename... Args>
		FSM::TransitionID	AddTransition(moe::FSM::StateID fromStateID, moe::FSM::StateID toStateID, Args... ctorParams)
		{
			static_assert(std::is_base_of<ITransition, TransitionType>::value, "Trying to add a transition that doesn't implement ITransition interface");

			MOE_DEBUG_ASSERT(HasState(fromStateID) && HasState(toStateID));

			StateDataTest& stateData = m_stateData[fromStateID];

			// TODO: encapsulate this unique ptr
			stateData.m_transitionData.EmplaceBack(std::make_unique<TransitionType>(std::forward<Args>(ctorParams)...), toStateID);

			return stateData.m_transitionData.Size() - 1;
		}

	};
}


