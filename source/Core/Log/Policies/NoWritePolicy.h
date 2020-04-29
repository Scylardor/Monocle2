#ifndef MOE_NO_WRITE_POLICY_H_
#define MOE_NO_WRITE_POLICY_H_

#ifdef MOE_STD_SUPPORT
#include <string>

#include "Monocle_Core_Export.h"

// A write policy that does nothing.
namespace moe
{
	class Monocle_Core_API NoWritePolicy
	{
	public:
		NoWritePolicy() {}

		static void Write(const std::string&) {}
	};
}
#endif // MOE_STD_SUPPORT

#endif // MOE_NO_WRITE_POLICY_H_