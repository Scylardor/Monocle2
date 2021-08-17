#pragma once

namespace moe
{

	template <typename Derived>
	struct CRTP
	{
	protected:

		Derived& Underlying() { return static_cast<Derived&>(*this); }
		Derived const& Underlying() const { return static_cast<Derived const&>(*this); }

		// To protect from misuse
		CRTP() {}
		friend Derived;
	};


#define MOE_CRTP_IMPL(Method, ...) \
	this->Underlying().Method##Impl(##__VA_ARGS__)

#define MOE_CRTP_IMPL_VARIADIC(Method, Ts, args) \
	this->Underlying().Method##Impl(std::forward<Ts>(args)...)


#define MOE_CRTP_IMPL_VARIADIC_TEMPLATE(Method, T1, Ts, args) \
	this->Underlying().template Method##Impl<T1>(std::forward<Ts>(args)...)


#define MOE_CRTP_IMPL_VARIADIC_2(Method, Ts, param1, args) \
	this->Underlying().Method##Impl(param1, std::forward<Ts>(args)...)

}
