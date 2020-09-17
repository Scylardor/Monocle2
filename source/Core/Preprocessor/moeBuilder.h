// Monocle Game Engine source files

#pragma once

/* A macro we use to ease the implementation of the Builder pattern.
 *
 * The Builder pattern is very handy to create objects setting only parameters we want.
 *
 * It avoids us having to call a constructor with a bunch of parameters we don't care about
 * just to get to the relevant ones.
 *
 */

#include "Core/Preprocessor/moeJoin.h"

#include <array> // For MOE_BUILDER_ARRAY_PROPERTY


#define MOE_BUILDER_INTERNAL_FUNC(PropertyName) \
	template <typename T> \
	auto& PropertyName(T&& value) { \
		MOE_JOIN(m_, PropertyName) = std::forward<T>(value); \
		return *this; \
	}

#define MOE_BUILDER_PROPERTY(PropertyType, PropertyName, PropertyValue) \
	PropertyType MOE_JOIN(m_, PropertyName){PropertyValue}; \
	MOE_BUILDER_INTERNAL_FUNC(PropertyName)


#define MOE_BUILDER_ARRAY_PROPERTY(PropertyType, ArraySize, PropertyName, PropertyValue) \
	std::array<PropertyType, ArraySize> MOE_JOIN(m_, PropertyName){PropertyValue}; \
	MOE_BUILDER_INTERNAL_FUNC(PropertyName)