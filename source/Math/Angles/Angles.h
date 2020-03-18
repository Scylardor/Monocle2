// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLM
#include "../GLM/Angles_glm.h"
#endif // MOE_GLM

namespace moe
{
	using Degs_f = Degs<float>;
	using Rads_f = Rads<float>;
}


/**
 * \brief Function that allows you to initialize a float Degrees struct with code like "45.0_degf" for example
 * \param degs The number of degrees
 * \return Float Degrees struct containing this number of degrees
 */
moe::Degs_f operator "" _degf(long double degs);


/**
 * \brief Function that allows you to initialize a float Degrees struct with code like "45_degf" for example
 * \param degs The number of degrees
 * \return Float Degrees struct containing this number of degrees
 */
moe::Degs_f operator "" _degf(unsigned long long degs);


/**
 * \brief Function that allows you to initialize a float Radians struct with code like "45.0_radf" for example
 * \param degs The number of radians
 * \return Float Radians struct containing this number of radians
 */
moe::Rads_f operator "" _radf(long double rads);


/**
 * \brief Function that allows you to initialize a float Radians struct with code like "45_radf" for example
 * \param degs The number of radians
 * \return Float Radians struct containing this number of radians
 */
moe::Rads_f operator "" _radf(unsigned long long rads);