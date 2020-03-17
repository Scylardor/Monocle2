// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Misc/moeError.h"
#include "Core/Misc/moeEnum.h"
#include "Core/Containers/Vector/Vector.h"
#include "Core/Containers/HashMap/HashMap.h"

#include "Input/Input.h"

#include <cstdint>

namespace moe
{
	typedef std::uint32_t	RawInputDeviceTypeID;

	template<class HandlerTypeT>
	struct RawInputHandlerTraits
	{
		using WindowHandle = void;
		using RawInputDeviceID = void;

		enum class ERawInputDeviceTypeID;
	};


	/* List of error codes for all steps of the raw input initialization process. */
	MOE_ENUM(RawInputHandlerErrors, std::uint8_t,
		GetRawInputDeviceListFailed = 1,
		GetRawInputDeviceInfoFailed,
		RegisterRawInputDevicesFailed
	);


	/*
		RAII class to deal with Raw Input (keyboard and mice).
		Put all the platform-specific code inside children classes.
	*/
	template <class DerivedT>
	class MOE_DLL_API IRawInputHandler
	{
	public:
		using HandlerTraits			= typename RawInputHandlerTraits<DerivedT>;
		using WindowHandle			= typename HandlerTraits::WindowHandle;
		using ERawInputDeviceTypeID	= typename HandlerTraits::ERawInputDeviceTypeID;
		using RawInputDeviceID		= typename HandlerTraits::RawInputDeviceID;

		/* A raw input device is primarily defined by an ID and a name. */
		class RawInputDevice
		{
		public:

			RawInputDevice(ERawInputDeviceTypeID deviceKindId, RawInputDeviceID id, LogicalDeviceID logicalId, std::wstring&& deviceName) :
				m_deviceKindId(deviceKindId), m_deviceId(id), m_logicalId(logicalId), m_deviceName(std::move(deviceName))
			{}


			RawInputDeviceID	GetID() const
			{
				return m_deviceId;
			}


			ERawInputDeviceTypeID	GetKind() const
			{
				return m_deviceKindId;
			}


			const std::wstring&	GetDeviceName() const
			{
				return m_deviceName;
			}


			ERawInputDeviceTypeID	m_deviceKindId = ERawInputDeviceTypeID::DeviceType_Unknown;
			RawInputDeviceID		m_deviceId;
			LogicalDeviceID			m_logicalId;
			std::wstring			m_deviceName;
		};


		virtual ~IRawInputHandler() = default;


		Error	RegisterWindowRawInputDevices(WindowHandle winHandle)
		{
			return Derived().RegisterWindowRawInputDevices(winHandle);
		}


		Error	UnregisterWindowRawInputDevices()
		{
			return Derived().UnregisterWindowRawInputDevices();
		}





	protected:

		void	RegisterRawInputDevice(RawInputDevice&& newDevice)
		{
			m_rawInputDevices[newDevice.GetKind()].PushBack(std::move(newDevice));
		}


	private:

		DerivedT&	Derived()
		{
			return static_cast<DerivedT&>(*this);
		}

		const DerivedT&	Derived() const
		{
			return static_cast<const DerivedT&>(*this);
		}


		HashMap<ERawInputDeviceTypeID, Vector<RawInputDevice>>	m_rawInputDevices;
	};
}


