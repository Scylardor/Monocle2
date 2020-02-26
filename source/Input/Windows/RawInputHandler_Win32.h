// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Input/RawInputHandlerInterface.h"

#include <Windows.h> // HWND


namespace moe
{
	class Win32RawInputHandler;

	template<>
	struct RawInputHandlerTraits<Win32RawInputHandler>
	{
		using WindowHandle = HWND;
		using RawInputDeviceID = HANDLE;

		enum class ERawInputDeviceTypeID : RawInputDeviceTypeID
		{
			DeviceType_Keyboard	= RIM_TYPEKEYBOARD,
			DeviceType_Mouse	= RIM_TYPEMOUSE,
			DeviceType_Unknown	= (RawInputDeviceTypeID)-1
		};

	};



	/*
		Win32 raw input handler
		Will listen for keyboards, mice.
		Additional features may come later.
	*/
	class MOE_DLL_API Win32RawInputHandler : public IRawInputHandler<Win32RawInputHandler>
	{
	public:

		typedef HWND	WindowHandle;

		Error	RegisterWindowRawInputDevices(WindowHandle winHandle);


		Error	UnregisterWindowRawInputDevices();

	private:
		using Super = IRawInputHandler<Win32RawInputHandler>;

		#ifndef MOE_SHIPPING
		static std::string	GetErrorFormatString(const char* format);
		#endif

		static Error		GetDeviceName(RAWINPUTDEVICELIST& rawInputDeviceList, std::wstring& deviceName);

		static const UINT	s_errorResult = (UINT)-1;
	};

}