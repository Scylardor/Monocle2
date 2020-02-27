// Monocle Game Engine source files - Alexandre Baron

#pragma once

/* These are some convenience preprocessor functions you can use to define debug-only or debug-excluded features. */

#ifdef MOE_DEBUG
# define	MOE_NOT_DEBUG(x)
#else
# define	MOE_NOT_DEBUG(x) x
#endif // MOE_DEBUG