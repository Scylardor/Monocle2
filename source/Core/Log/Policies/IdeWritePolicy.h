#ifndef MOE_IDE_WRITE_POLICY_H_
#define MOE_IDE_WRITE_POLICY_H_

// Generic header of IdeWritePolicy
// This is where conditional includes take place
// to avoid polluting client code.

#ifdef MOE_WINDOWS
#include "Core/Log/Policies/Windows/Win_IdeWritePolicy.h"
#elif MOE_LINUX
#include "Core/Log/Policies/Linux/Linux_IdeWritePolicy.h"
#endif

#endif // MOE_IDE_WRITE_POLICY_H_
