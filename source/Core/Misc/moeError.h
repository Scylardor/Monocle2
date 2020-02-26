// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Preprocessor/moeDLLVisibility.h"

#include <cstdint>

#ifdef MOE_STD_SUPPORT
#include <string>
#endif

/* TODO: maybe put this something else */
#ifndef MOE_SHIPPING
#define COMMA ,
#define MOE_ERROR_STRING(x) COMMA std::move(x)
#define MOE_DBG_PARAM(x) COMMA x
#else
#define MOE_ERROR_STRING(x)
#define MOE_DBG_PARAM(x)
#endif

namespace moe
{
	/*
		Monocle error class
		This is a simple wrapper over a return error code and an optional error message string.
		An empty, default-initialized Error is considered as "absence of error".
		The string is stripped in Shipping builds, leaving only the error ID.
	*/
	class MOE_DLL_API Error
	{
	public:
		typedef std::uint32_t	Code;

		Error() = default; // Returns a "NoError" error by default

		#ifndef MOE_SHIPPING

		Error(Code code, const char* str) :
			m_code(code), m_errorMsg(str)
		{}

		Error(Code code, std::string&& str) :
			m_code(code), m_errorMsg(std::move(str))
		{}

		#else

		Error(Code code) :
			m_code(code)
		{}

		#endif



		operator bool() const
		{
			return HasError();
		}


		Code	GetCode() const
		{
			return m_code;
		}


		bool	HasError() const
		{
			return m_code != s_noError;
		}


		#ifndef MOE_SHIPPING
		const std::string&	GetMessage() const
		{
			return m_errorMsg;
		}
		#endif


		// Helper static functions to make calling code more explicit and self documenting.
		static Error None()
		{
			return Error(s_noError MOE_ERROR_STRING("No Error"));
		}

		static Error Some()
		{
			return Error(s_someError MOE_ERROR_STRING("Unspecified Error"));
		}

	private:
		Code	m_code = s_noError;
		std::string	m_errorMsg;

		static const Code s_noError = 0;
		static const Code s_someError = (Code)-1;	// Some error, for when we don't specifically care about the code value or reason
	};
}