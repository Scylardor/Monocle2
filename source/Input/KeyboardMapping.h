// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include <cstdint> // std::uint8_t
#include "Core/Misc/moeEnum.h"
#include "Core/Containers/Vector/Vector.h"
#include "Core/Delegates/Event.h"

#include "Input/Input.h"
#include "Input/InputDescriptors.h"


namespace moe
{
	/*
		Describes a keyboard mapping:
		- a keycode
		- whether it's meant to be fired on key pressed, repeated, or released, or any combination of the three
	*/
	struct KeyboardMappingDesc
	{
		KeyboardMappingDesc(Keycode code, ButtonState state, LogicalDeviceID logicalKb = LogicalDeviceID::Keyboard_0) :
			m_eventDesc(code, state), m_keyboardLogicalID(logicalKb)
		{}

		bool	Matches(const KeyboardEventDesc& inDesc) const
		{
			bool matches = (m_eventDesc.m_code == inDesc.m_code && (m_eventDesc.m_state & inDesc.m_state));
			return matches;
		}

		KeyboardEventDesc	m_eventDesc;
		LogicalDeviceID		m_keyboardLogicalID = LogicalDeviceID::Keyboard_0;
	};


	/*
		Declares a mapping to a keyboard event
	*/
	class KeyboardMapping
	{
	public:

		KeyboardMapping(InputMappingID mapID, const KeyboardMappingDesc& mapDesc) :
			m_id(mapID), m_mappingDesc(mapDesc)
		{}



		void	SetDescription(const KeyboardMappingDesc& mapDesc)
		{
			m_mappingDesc = mapDesc;
		}

		const KeyboardMappingDesc&	Description() const
		{
			return m_mappingDesc;
		}


		InputMappingID	GetID() const { return m_id; }

	private:
		InputMappingID	m_id;
		KeyboardMappingDesc	m_mappingDesc;
	};


}