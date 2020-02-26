#ifndef MOE_LOG_H_
#define MOE_LOG_H_

// This is the "include all things" header for Monocle logging.
// It includes commonly used logger interfaces, logging utilities, log policies...
// And the usually useful macros to log more easily.

// The logger basically includes everything else
#include "Core/Log/moeLogger.h"
#include "Core/Log/moeLogMacros.h"

// Logger Policies
#include "Core/Log/Policies/NoFilterPolicy.h"
#include "Core/Log/Policies/SeverityFilterPolicy.h"

#include "Core/Log/Policies/NoFormatPolicy.h"
#include "Core/Log/Policies/DebuggerFormatPolicy.h"

#include "Core/Log/Policies/CaptureWritePolicy.h"
#include "Core/Log/Policies/FileWritePolicy.h"
#include "Core/Log/Policies/OutStreamWritePolicy.h"
#include "Core/Log/Policies/IdeWritePolicy.h"

#endif // MOE_LOG_H_