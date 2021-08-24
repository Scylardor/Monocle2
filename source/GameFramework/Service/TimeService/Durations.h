// Monocle source files - Alexandre Baron

#pragma once
#include <chrono>

namespace moe
{

	using ns = std::chrono::duration<long long, std::nano>;
	using us = std::chrono::duration<long long, std::micro>;
	using ms = std::chrono::duration<long long, std::milli>;
	using sec = std::chrono::duration<float>;
}
