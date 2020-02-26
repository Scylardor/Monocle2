/*
	Implementation of a callback delegate using variadic template parameters
	Heavily inspired by https://nikitablack.github.io/2016/04/26/stdfunction-as-delegate.html
*/

#pragma once

#include <functional> // std::function
#include <utility> // std::index_sequence_for

// We're going to use a metaprogramming integer sequence to statically generate std::placeholders stubs to pass to std::bind
// in order to construct our delegates.
namespace moe
{
	template<size_t>
	struct DelegatePlaceholder {};

	template <size_t N>
	DelegatePlaceholder<N + 1> GetNextPlaceholder()
	{
		return{};
	}
}

// Using std::is_placeholder, we're able to tell that for example, EventPlaceholder<2> is to be replaced by placeholders::_2, etc...
namespace std
{
	template<size_t N>
	struct is_placeholder<moe::DelegatePlaceholder<N>> : public integral_constant<size_t, N> {};
}


// We're going to use a metaprogramming integer sequence to statically generate std::placeholders stubs to pass to std::bind
// in order to construct our delegates.
namespace moe
{
	template<typename FunctionType>
	class Delegate;

	template <typename Ret, typename... Args>
	class Delegate<Ret(Args...)>
	{
		using DelegateType = std::function<Ret(Args...)>;

		DelegateType	m_func = nullptr;

	public:

		Delegate() = default;

		Delegate(Ret(*funcPtr)(Args...)) :
			m_func(funcPtr)
		{}


		template<class Callee>
		Delegate(Callee* obj, Ret(Callee::*funcPtr)(Args...))
		{
			m_func = Bind(obj, funcPtr, std::index_sequence_for<Args...>{});
		}


		// Free function version
		template<Ret(*funcPtr)(Args...)>
		void	Set()
		{
			m_func = funcPtr;
		}

		// Member function version
		template<class Callee, Ret(Callee::*funcPtr)(Args...)>
		void	Set(Callee* obj)
		{
			m_func = Bind(obj, funcPtr, std::index_sequence_for<Args...>{});
		}

		Ret	operator()(Args... args)
		{
			return m_func(args...);
		}

		bool	IsSet() const { return m_func != nullptr; }

	private:


		/*
			Using std::index_sequence, GetNextPlaceholder<Idx>() will get expanded into appropriate std::placeholders::_Idx.
		*/
		template <class Callee, std::size_t... Idx>
		static DelegateType	Bind(Callee* obj, Ret(Callee::*funcPtr)(Args...), std::index_sequence<Idx...>)
		{
			return std::bind(funcPtr, obj, GetNextPlaceholder<Idx>()...);
		}
	};
}