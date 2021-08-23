// Monocle Game Engine source files - Alexandre Baron

#pragma once
#include <stdexcept>

namespace moe
{
	class RuntimeException : public std::runtime_error
	{
	public:

		RuntimeException(char const * reason) :
			std::runtime_error(reason)
		{}

		RuntimeException(std::string const& reason) :
			std::runtime_error(reason)
		{}

		[[nodiscard]] const char*	Reason() const noexcept
		{
			return what();
		}
	};
}
