// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include <cstdint>
#include <limits>


#define MOE_MOVE(var) var = std::move(rhs.var)

namespace moe
{
		using std::int8_t;
		using std::int16_t;
		using std::int32_t;
		using std::int64_t;

		using std::uint8_t;
		using std::uint16_t;
		using std::uint32_t;
		using std::uint64_t;

		using byte_t = uint8_t;


		template<typename T>
		auto	MaxValue()
		{
			return std::numeric_limits<T>::max();
		}

		template<typename T>
		auto	MinValue()
		{
			return std::numeric_limits<T>::min();
		}
}
