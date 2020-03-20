// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/moeNamedType.h"

#include <glm/glm.hpp> // glm::radians, glm::degrees

namespace moe
{
	// Necessary to forward declare Rads for the Rad to Deg conversion
	template <typename T>
	struct Rads;


	struct DegsPhantom {}; // for named type

	/**
	* \brief Simple struct to encapsulate degrees. This idea was taken from Magnum Engine.
	* \tparam T : underlying type of stored degrees
	*/
	template <typename T>
	struct Degs : NamedType<T, DegsPhantom>
	{
		static_assert(std::is_floating_point<T>::value, "Unsupported template type for Degrees.");

		explicit Degs(T val) :
			NamedType(val)
		{}

		explicit Degs(Rads<T> rads) :
			NamedType(glm::degrees((T)rads))
		{}


		Degs&	operator=(Rads<T> rads)
		{
			m_value = glm::degrees((T)rads);
			return *this;
		}

	};


	struct RadsPhantom {}; // for named type

	/**
	 * \brief Simple struct to encapsulate radians. This idea was taken from Magnum Engine.
	 * The rationale is that a very common mistake with math functions is to pass degrees to functions that expect radians.
	 * By carefully designing our API to take Radians and make the Degrees -> Radians conversion painless, we can make it much harder to do this dumb error.
	 * \tparam T : underlying type of stored radians
	 */
	template <typename T>
	struct Rads : NamedType<T, RadsPhantom>
	{
		static_assert(std::is_floating_point<T>::value, "Unsupported template type for Radians.");

		explicit Rads(T val) :
			NamedType(val)
		{}

		Rads(Degs<T> degs) :
			NamedType(glm::radians((T)degs))
		{}


		Rads&	operator=(Degs<T> degs)
		{
			m_value = glm::radians((T)degs);
			return *this;
		}
	};

	using Degs_f = Degs<float>;
	using Rads_f = Rads<float>;
}
