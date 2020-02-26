#ifndef MOE_CAPTURE_WRITE_POLICY_H_
#define MOE_CAPTURE_WRITE_POLICY_H_

// CaptureWritePolicy is a logging write-policy that captures its input by simply appending it to a string.
// Mainly useful to check what you received.

// TODO: GetCapturedOutput API isn't super clean because we can't export std::string in a DLL because of potential ABI problems
// cf. this warning:
// warning C4251: 'moe::CaptureWritePolicy::m_captured': class 'std::basic_string<char,std::char_traits<char>,std::allocator<char>>' needs to have dll-interface to be used by clients of class 'moe::CaptureWritePolicy'
// it should be OK if we build linking exe and linked DLL with the same compiler, but ultimately we can't use std for this.
#include "Core/Preprocessor/moeDLLVisibility.h"

#ifdef MOE_STD_SUPPORT
#include <string>

namespace moe
{
	class CaptureWritePolicy
	{
	public:
		CaptureWritePolicy() {}

		MOE_DLL_API void	Write(const std::string& message);

		MOE_DLL_API void    Clear();

		// Not the best interface. Wish we could return const std::string&. But not across a DLL boundary
		MOE_DLL_API const char* GetCapturedOutput() const;

	private:
		std::string m_captured;
	};
}

#endif // MOE_STD_SUPPORT

#endif // MOE_CAPTURE_WRITE_POLICY_H_