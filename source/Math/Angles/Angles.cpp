// Monocle Game Engine source files - Alexandre Baron

#include "Angles.h"

moe::Degs_f operator "" _degf(long double degs)
{
	return moe::Degs_f(float(degs));
}


moe::Degs_f operator ""_degf(unsigned long long degs)
{
	return moe::Degs_f(float(degs));
}


moe::Rads_f operator "" _radf(long double rads)
{
	return moe::Rads_f(float(rads));
}


moe::Rads_f operator ""_radf(unsigned long long rads)
{
	return moe::Rads_f(float(rads));
}
