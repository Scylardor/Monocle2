// Monocle Game Engine source files - Alexandre Baron

#include "Input/Windows/RawInputHandler_Win32.h"

#include "Core/Misc/Windows/GetLastErrorAsString.h"
#include "Core/Misc/moeCountof.h"
#include "Core/StringFormat/moeStringFormat.h"
#include "Core/Log/moeLog.h"
#include "Core/Containers/Vector/Vector.h"


// Combination of useful defines to make the code more readable (avoids magic numbers)
// cf. https://docs.microsoft.com/fr-fr/windows-hardware/drivers/hid/top-level-collections-opened-by-windows-for-system-use
// and https://docs.microsoft.com/en-us/windows/desktop/dxtecharts/taking-advantage-of-high-dpi-mouse-movement
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC		((USHORT) 0x01)
#endif

#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE		((USHORT) 0x02)
#endif

#ifndef HID_USAGE_GENERIC_KEYBOARD
#define HID_USAGE_GENERIC_KEYBOARD	((USHORT) 0x06)
#endif


#if defined(MOE_DEBUG) || defined(MOE_DIAGNOSTIC)
#define MOE_INPUT_DEBUG_INFO(...) MOE_INFO(moe::ChanInput, ##__VA_ARGS__)
#else
#define MOE_INPUT_DEBUG_INFO(...)
#endif



namespace moe
{
	Error Win32RawInputHandler::RegisterWindowRawInputDevices(WindowHandle winHandle)
	{
		UINT result;

		// raw devices count
		UINT nDevices = 0;

		// get number of raw devices, including fake ones for RDP etc, this allows  us
		// to allocate the right amount of memory, since we're only asking for how many are attached.
		result = GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST));

		MOE_INPUT_DEBUG_INFO("Number of detected raw devices: %u", nDevices);

		// this would be bad. possibly permissions.
		if (result == s_errorResult)
		{
			return Error(RawInputHandlerErrors::GetRawInputDeviceListFailed MOE_ERROR_STRING(GetErrorFormatString("Failed to get raw input device list: %s")));
		}

		// now get the actual list of raw devices
		Vector<RAWINPUTDEVICELIST> rawInputDeviceList(nDevices);
		result = GetRawInputDeviceList(&rawInputDeviceList[0], &nDevices, sizeof(RAWINPUTDEVICELIST));
		if (result == s_errorResult)
		{
			return Error(RawInputHandlerErrors::GetRawInputDeviceListFailed MOE_ERROR_STRING(GetErrorFormatString("Failed to get raw input device list: %s")));
		}


		LogicalDeviceID nextKeyboardLogicalID = LogicalDeviceID::Keyboard_0;
		LogicalDeviceID nextMouseLogicalID = LogicalDeviceID::Mouse_0;

		// We can now populate our data about raw input devices
		for (UINT iDev = 0; iDev < nDevices; ++iDev)
		{
			// First get the device name
			std::wstring deviceName;
			Error error = GetDeviceName(rawInputDeviceList[iDev], deviceName);

			if (error)
			{
				MOE_INPUT_DEBUG_INFO("Failed to get device %d name. Trying to get more info.", iDev + 1);
			}

			// Now get the device info
			// setup buffer to receive device info
			RID_DEVICE_INFO ridDeviceInfo;
			ridDeviceInfo.cbSize = sizeof(RID_DEVICE_INFO);

			// this tells the api how big the structure is
			UINT nDeviceBufferLength = ridDeviceInfo.cbSize;

			// fetch the device info for this raw device
			result = GetRawInputDeviceInfo(rawInputDeviceList[iDev].hDevice, RIDI_DEVICEINFO, &ridDeviceInfo, &nDeviceBufferLength);

			if (result == s_errorResult)
			{
				MOE_INPUT_DEBUG_INFO("Failed to get device %d device info. Skipping this device.", iDev + 1);
				continue;
			}

			if (ridDeviceInfo.dwType == RIM_TYPEKEYBOARD)
			{
				MOE_INPUT_DEBUG_INFO("Registering KEYBOARD device handle 0x%x (%d) - device name: %s\n", rawInputDeviceList[iDev].hDevice, iDev + 1, deviceName);
				MOE_INPUT_DEBUG_INFO("dwKeyboardMode %d\n", ridDeviceInfo.keyboard.dwKeyboardMode);
				MOE_INPUT_DEBUG_INFO("dwNumberOfFunctionKeys %d\n", ridDeviceInfo.keyboard.dwNumberOfFunctionKeys);
				MOE_INPUT_DEBUG_INFO("dwNumberOfIndicators %d\n", ridDeviceInfo.keyboard.dwNumberOfIndicators);
				MOE_INPUT_DEBUG_INFO("dwNumberOfKeysTotal %d\n", ridDeviceInfo.keyboard.dwNumberOfKeysTotal);
				MOE_INPUT_DEBUG_INFO("dwType %d\n", ridDeviceInfo.keyboard.dwType);
				MOE_INPUT_DEBUG_INFO("dwSubType %d\n", ridDeviceInfo.keyboard.dwSubType);

				// Throttle the maximum number of keyboards detected
				if (nextKeyboardLogicalID != +LogicalDeviceID::Keyboard_MAX)
				{
					RawInputDevice keyboardDevice(ERawInputDeviceTypeID::DeviceType_Keyboard, (RawInputDeviceID)rawInputDeviceList[iDev].hDevice, nextKeyboardLogicalID, std::move(deviceName));
					RegisterRawInputDevice(std::move(keyboardDevice));
					nextKeyboardLogicalID++;
				}
				else
				{
					MOE_INPUT_DEBUG_INFO("Keyboard device skipped (max number of keyboards reached).");
				}
			}
			else if (ridDeviceInfo.dwType == RIM_TYPEMOUSE)
			{
				MOE_INPUT_DEBUG_INFO("Registering MOUSE device handle 0x%x (%d) - device name: %s\n", rawInputDeviceList[iDev].hDevice, iDev + 1, deviceName);
				MOE_INPUT_DEBUG_INFO("dwId %d\n", ridDeviceInfo.mouse.dwId);
				MOE_INPUT_DEBUG_INFO("dwNumberOfButtons %d\n", ridDeviceInfo.mouse.dwNumberOfButtons);
				MOE_INPUT_DEBUG_INFO("dwSampleRate %d\n", ridDeviceInfo.mouse.dwSampleRate);
				MOE_INPUT_DEBUG_INFO("fHasHorizontalWheel ? %s\n", (ridDeviceInfo.mouse.fHasHorizontalWheel ? "yes" : "no"));

				if (nextMouseLogicalID != +LogicalDeviceID::Mouse_MAX)
				{
					RawInputDevice mouseDevice(ERawInputDeviceTypeID::DeviceType_Mouse, (RawInputDeviceID)rawInputDeviceList[iDev].hDevice, nextMouseLogicalID, std::move(deviceName));
					RegisterRawInputDevice(std::move(mouseDevice));
					nextMouseLogicalID++;
				}
				else
				{
					MOE_INPUT_DEBUG_INFO("Mouse device skipped (max number of mice reached).");
				}
			}
		}

		// Now that we iterated on available raw input devices information,
		// register ourselves to the window to receive WM_INPUT events.

		// Only listen for keyboards and mice for now
		RAWINPUTDEVICE rids[2];

		// Note: we can use a variety of flags. Here use RIDEV_DEVNOTIFY to be notified of WM_INPUT_DEVICE_CHANGE events.
		// See the full list at https://docs.microsoft.com/en-us/windows/desktop/api/winuser/ns-winuser-tagrawinputdevice#members
		rids[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		rids[0].usUsage = HID_USAGE_GENERIC_MOUSE;
		rids[0].dwFlags = RIDEV_DEVNOTIFY;
		rids[0].hwndTarget = winHandle;

		rids[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
		rids[1].usUsage = HID_USAGE_GENERIC_KEYBOARD;
		rids[1].dwFlags = RIDEV_DEVNOTIFY;
		rids[1].hwndTarget = winHandle;

		if (FALSE == RegisterRawInputDevices(rids, (UINT)Countof(rids), sizeof(RAWINPUTDEVICE)))
		{
			return Error(RawInputHandlerErrors::RegisterRawInputDevicesFailed MOE_ERROR_STRING(GetErrorFormatString("Failed to register raw input devices: %s")));
		}

		MOE_INPUT_DEBUG_INFO("Raw input device registering successful!");


		return Error::None();
	}


	Error Win32RawInputHandler::UnregisterWindowRawInputDevices()
	{
		RAWINPUTDEVICE rids[2];

		rids[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		rids[0].usUsage = HID_USAGE_GENERIC_MOUSE;
		rids[0].dwFlags = RIDEV_REMOVE;
		rids[0].hwndTarget = NULL;

		rids[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
		rids[1].usUsage = HID_USAGE_GENERIC_KEYBOARD;
		rids[1].dwFlags = RIDEV_REMOVE;
		rids[1].hwndTarget = NULL;

		if (FALSE == RegisterRawInputDevices(rids, (UINT)Countof(rids), sizeof(RAWINPUTDEVICE)))
		{
			return Error(RawInputHandlerErrors::RegisterRawInputDevicesFailed MOE_ERROR_STRING(GetErrorFormatString("Failed to unregister raw input devices : %s")));
		}

		MOE_INPUT_DEBUG_INFO("Raw input device unregistering successful!");

		return Error::None();
	}


	Error Win32RawInputHandler::GetDeviceName(RAWINPUTDEVICELIST & rawInputDeviceList, std::wstring& deviceName)
	{
		// First get the device name length
		UINT nDeviceBufferLength = 0;

		UINT result = GetRawInputDeviceInfo(rawInputDeviceList.hDevice, RIDI_DEVICENAME, NULL, &nDeviceBufferLength);

		if (result == s_errorResult)
		{
			return Error(RawInputHandlerErrors::GetRawInputDeviceInfoFailed MOE_ERROR_STRING(GetErrorFormatString("Failed to read raw input device name: %s")));
		}

		// allocate memory for device name (wide char)
		std::wstring nameBuffer(nDeviceBufferLength + 1, '\0');

		// Now get the device name
		result = GetRawInputDeviceInfo(rawInputDeviceList.hDevice, RIDI_DEVICENAME, &nameBuffer[0], &nDeviceBufferLength);
		if (result == s_errorResult)
		{
			return Error(RawInputHandlerErrors::GetRawInputDeviceInfoFailed MOE_ERROR_STRING(GetErrorFormatString("Failed to read raw input device name: %s")));
		}

		deviceName = std::move(nameBuffer);

		return Error::None();
	}


	#ifndef MOE_SHIPPING
	std::string Win32RawInputHandler::GetErrorFormatString(const char * format)
	{
		std::string errorMsg;
		StringFormat(errorMsg, format, GetLastErrorAsString());
		return errorMsg;
	}
	#endif
}
