// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/moeNamedType.h"

// Use pragmas to disable warnings coming from GLM.
#pragma warning(push, 0)
#include <glm/glm/vec2.hpp>
#include <glm/glm.hpp> // glm::radians, glm::degrees
#pragma warning(pop)

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


		Degs	operator*(T val)
		{
			return Degs(m_value * val);
		}


		/**
		 * \brief Allows to write things like "-55_degf" without the compiler yelling because Degs constructor is explicit.
		 * \return The same value, turned negative
		 */
		Degs	operator-()
		{
			return Degs(-m_value);
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


		/**
		 * \brief Allows to write things like "-55_radf" without the compiler yelling because Rads constructor is explicit.
		 * \return The same value, turned negative
		 */
		Rads	operator-()
		{
			return Rads(-m_value);
		}
	};

	using Degs_f = Degs<float>;
	using Rads_f = Rads<float>;
}
