// Monocle Game Engine source files - Alexandre Baron
//
#include "GameFramework/FSM/FSM.h"
#include "Core/Preprocessor/moeAssert.h"

namespace moe
{
	bool FSM::HasState(FSM::StateID id) const
	{
		return (m_stateData.Find((int)id) != m_stateData.End());
	}

	void FSM::StartState(StateID id)
	{
		if (m_currentStateID != s_UninitializedState)
		{
			StateDataTest& exitingStateData = ModifyStateData(m_currentStateID);
			exitingStateData.m_state->OnStateExit(*this, m_currentStateID);
		}

		StateDataTest& enteringStateData = ModifyStateData(id);
		enteringStateData.m_state->OnStateEnter(*this, id);
		m_currentStateID = id;
	}


	void FSM::RemoveState(StateID removedId)
	{
		MOE_DEBUG_ASSERT(HasState(removedId));

		// First remove the given state from the states hash map
		m_stateData.Erase(removedId);

		// Then look for transitions pointing to this state in all other states
		for (auto& fsmDataIt : m_stateData)
		{
			StateDataTest& stateData = fsmDataIt.second;
			StateDataTest::StateTransitionsData& transData = stateData.m_transitionData;
			if (false == stateData.m_transitionData.Empty())
			{
				transData.Erase(
					std::remove_if(transData.Begin(), transData.End(), [removedId](const auto& td) { return td.m_destination == removedId; }),
					transData.End()
				);
			}
		}
	}


	FSM::TransitionNumber FSM::RemoveStateTransition(FSM::StateID stateId, FSM::TransitionID transId)
	{
		MOE_DEBUG_ASSERT(HasState(stateId));
		StateDataTest::StateTransitionsData& stateTransData = ModifyStateData(stateId).m_transitionData;

		MOE_DEBUG_ASSERT(stateTransData.Size() > transId);

		stateTransData.EraseBySwapAt(transId);

		return stateTransData.Size();
	}


	FSM::TransitionNumber FSM::GetStateNumberOfTransitions(StateID id) const
	{
		MOE_DEBUG_ASSERT(HasState(id));
		return GetStateData(id).m_transitionData.Size();
	}


	void FSM::StopFSM()
	{
		if (IsRunning())
		{
			ModifyStateData(m_currentStateID).m_state->OnStateExit(*this, m_currentStateID);
			m_currentStateID = s_UninitializedState;
		}
	}


	void FSM::Clear()
	{
		m_stateData.Clear();
		m_currentStateID = s_UninitializedState;
		m_stateNbr = 0;
		m_transitionNbr = 0;
	}

}

