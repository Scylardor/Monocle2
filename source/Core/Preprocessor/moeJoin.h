#ifndef MOE_JOIN_H_
#define MOE_JOIN_H_

// This kind of strange idiom allows us to even join macro values together (e.g. __FILE__ ...)
// Needs two (!) layers of indirection for macros to expand correctly in that case
#define MOE_JOIN_INTERNAL2(a, b) a ## b
#define MOE_JOIN_INTERNAL(a, b) MOE_JOIN_INTERNAL2(a, b)
#define MOE_JOIN(a, b) MOE_JOIN_INTERNAL(a, b)

#endif // MOE_JOIN_H_
