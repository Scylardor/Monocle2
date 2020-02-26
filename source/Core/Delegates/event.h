/*
	Implementation of a multicast-event system
	Heavily inspired by https://nikitablack.github.io/2016/04/26/stdfunction-as-delegate.html
*/

#pragma once

#include "Core/Containers/Vector/Vector.h"
#include "Core/Delegates/Delegate.h"

#include <algorithm> // std::find_if

namespace moe
{
	typedef	std::size_t	EventDelegateID;

	template<typename FunctionType>
	class Event;

	/*
		An event you can bind delegates to, that will be called when the event is Broadcast'ed.
		This implementation does not prevent you to call the same function multiple times for the same event, know what you're doing.
		It keeps track of a "delegate ID" it returns you when you add a new delegate so that you can remove your delegate if you wish.
	*/
	template <typename Ret, typename... Args>
	class Event<Ret(Args...)>
	{
	public:
		using DelegateType = Delegate<Ret(Args...)>;


		EventDelegateID	AddDelegate(const DelegateType& func)
		{
			m_delegates.EmplaceBack(m_nextID, func);
			return m_nextID++;
		}

		EventDelegateID	AddDelegate(DelegateType&& func)
		{
			m_delegates.EmplaceBack(m_nextID, std::forward<DelegateType>(func));
			return m_nextID++;
		}


		// Free function version
		template<Ret(*funcPtr)(Args...)>
		EventDelegateID	Add()
		{
			return AddDelegate(funcPtr);
		}

		// Member function version
		template<class Callee, Ret(Callee::*funcPtr)(Args...)>
		EventDelegateID Add(Callee* obj)
		{
			return AddDelegate(DelegateType(obj, funcPtr));
		}


		/*
			This function performs a linear search for a delegate with this ID (O(N) complexity).
			The assumption is that it won't be used very often. It also expects to find the given ID.
		*/
		bool	Remove(EventDelegateID dlgtId)
		{
			auto dlgtStructIt = std::find_if(m_delegates.Begin(), m_delegates.End(), [dlgtId](const EventDelegate& dlgtStruct)
			{
				return (dlgtStruct.m_id == dlgtId);
			});

			bool found = dlgtStructIt != m_delegates.End();
			if (MOE_ASSERT(found))
			{
				m_delegates.EraseBySwap(dlgtStructIt); // Changes the order of the vector but we don't care !
			}

			return found;
		}


		/* Triggers all the delegates */
		void	Broadcast(Args... args)
		{
			for (auto& dlgtStruct : m_delegates)
			{
				dlgtStruct.m_delegate(args...);
			}
		}


		EventDelegateID	GetNextID() const { return m_nextID; }


	private:



		/* A simple struct holding the actual delegate and an ID */
		struct EventDelegate
		{
			EventDelegate() = default;
			EventDelegate(EventDelegateID id, const DelegateType& dlgt) :
				m_id(id), m_delegate(dlgt)
			{}
			EventDelegate(EventDelegateID id, DelegateType&& dlgt) :
				m_id(id), m_delegate(std::move(dlgt))
			{}

			DelegateType	m_delegate = nullptr;
			EventDelegateID	m_id = 0;
		};





		typedef moe::Vector<EventDelegate>	EventDelegates;
		EventDelegates	m_delegates;
		EventDelegateID	m_nextID = 0;
	};


}


