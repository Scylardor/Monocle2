// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "BetterEnums/enum.h"

/*
	Monocle better enums
	Based on BetterEnums header-only library.
	The goal is to provide a friendly and improved enum interface that allows easy enum iteration, stringization, etc.
	I also added basic operator overloading to allow enums to be used as bitflags.

	/!\ IMPORTANT THINGS TO KNOW:

	- Better Enums can't be used inside the scope of a class, so MOE_ENUMs lack this feature too. You have to declare them in a namespace (or global scope).

	- You can't use operators != or == like this : "if (myEnum == EnumType::EnumValue)" as of today. You have to do : "if (myEnum == +EnumType::EnumValue)".

	- Better Enums doesn't support the "trailing comma syntax" standard enums provide (i.e. ending the enum list with a comma).

	- To be able to use operators, you need to prefix the operand enum values with "+" (it is a Better Enum feature).

	Note : if you read the http://blog.bitwigglers.org post you'll realize I didn't implement the SFINAE for bitflags enums.
	It's because I thought it was too much added complexity for not a lot of gained value.
	Instead, I use DECLARE_MOE_ENUM_OPERATORS and DEFINE_MOE_ENUM_OPERATORS macros. When creating a new MOE_ENUM, you need to use both
	to use bitwise operations (DECLARE_MOE_ENUM_OPERATORS in the header, DEFINE_MOE_ENUM_OPERATORS in a translation unit, .cpp etc...).

	Better Enums is distributed under the terms of the 2-clause BSD license (see LICENSE in ThirdParty/BetterEnums).

	See also:
	- https://aantron.github.io/better-enums/
	- http://blog.bitwigglers.org/using-enum-classes-as-type-safe-bitmasks/
*/

#define MOE_ENUM(Enum, UnderlyingType, ...) \
BETTER_ENUM(Enum, UnderlyingType, ##__VA_ARGS__);

#define DECLARE_MOE_ENUM_OPERATORS(Enum)\
Enum	operator |(Enum lhs, Enum rhs);\
Enum	operator &(Enum lhs, Enum rhs);\
Enum	operator ^(Enum lhs, Enum rhs);\
Enum	operator ~(Enum rhs);\
Enum&	operator |=(Enum &lhs, Enum rhs);\
Enum&	operator &=(Enum &lhs, Enum rhs);\
Enum&	operator ^=(Enum &lhs, Enum rhs); \
Enum&	operator ++(Enum& rhs); /* prefix ++ */ \
Enum	operator ++(Enum& lhs, int); /* postfix ++ */ \
Enum&	operator --(Enum& rhs); /* prefix -- */ \
Enum	operator --(Enum& lhs, int); /* postfix -- */


#define DEFINE_MOE_ENUM_OPERATORS(Enum) \
Enum operator |(Enum lhs, Enum rhs) \
{ \
	return Enum::_from_integral(\
		lhs._to_integral() | rhs._to_integral() \
	); \
} \
Enum operator &(Enum lhs, Enum rhs) \
{ \
	return Enum::_from_integral(\
		lhs._to_integral() & rhs._to_integral() \
	); \
} \
Enum operator ^(Enum lhs, Enum rhs) \
{ \
	return Enum::_from_integral(\
		lhs._to_integral() ^ rhs._to_integral() \
	); \
} \
Enum operator ~(Enum rhs) \
{ \
	return Enum::_from_integral(\
		~rhs._to_integral() \
	); \
} \
Enum& operator |=(Enum &lhs, Enum rhs) \
{ \
	lhs = lhs | rhs; \
	return lhs; \
} \
Enum& operator &=(Enum &lhs, Enum rhs) \
{ \
	lhs = lhs & rhs; \
	return lhs; \
} \
Enum& operator ^=(Enum &lhs, Enum rhs) \
{ \
	lhs = lhs ^ rhs; \
	return lhs; \
} \
Enum& operator ++(Enum& rhs) /* prefix ++ */ \
{ \
	rhs = Enum::_from_integral(rhs._to_integral() + 1); \
	return rhs; \
} \
Enum operator ++(Enum& lhs, int) /* postfix ++ */ \
{ \
	Enum copy = lhs; \
	lhs = Enum::_from_integral(lhs._to_integral() + 1); \
	return copy; \
} \
Enum& operator --(Enum& rhs) /* prefix -- */ \
{ \
	rhs = Enum::_from_integral(rhs._to_integral() - 1); \
	return rhs; \
} \
Enum operator --(Enum& lhs, int) /* postfix -- */ \
{ \
	Enum copy = lhs; \
	lhs = Enum::_from_integral(lhs._to_integral() - 1); \
	return copy; \
}


