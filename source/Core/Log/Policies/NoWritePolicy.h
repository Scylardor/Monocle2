#ifndef MOE_NO_WRITE_POLICY_H_
#define MOE_NO_WRITE_POLICY_H_

#ifdef MOE_STD_SUPPORT
#include <string>

#include "Core/Preprocessor/moeDLLVisibility.h"

// A write policy that does nothing.
namespace moe
{
	class MOE_DLL_API NoWritePolicy
	{
	public:
		NoWritePolicy() {}

		static void Write(const std::string&) {}
	};
}
#endif // MOE_STD_SUPPORT

#endif // MOE_NO_WRITE_POLICY_H_