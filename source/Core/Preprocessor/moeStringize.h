#ifndef MOE_STRINGIZE_H_
#define MOE_STRINGIZE_H_

// Useful to stringize an expression
// Uses a level of indirection so it works even on macro values
#define MOE_STRINGIZE_INTERNAL(token) #token
#define MOE_STRINGIZE(expr) MOE_STRINGIZE_INTERNAL(expr)

#endif // MOE_STRINGIZE_H_