// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Preprocessor/moeDLLVisibility.h"
#include "Core/Containers/HashMap/HashMap.h"
#include "Core/Containers/Vector/Vector.h"
#include "Core/HashString/HashString.h"

#include "Input/KeyboardMapping.h"
#include "Input/InputDescriptors.h"
#include "Input/InputEventsSink.h"


namespace moe
{
	class MOE_DLL_API IInputHandler
	{
	public:

		typedef Delegate<void(InputMappingID)>	ActionMappingDelegate;


	private:
		typedef Event<void(InputMappingID)>		ActionMappingEvent;

		struct	ActionDescriptor
		{
			Vector<KeyboardMapping>	m_kbMaps;

			ActionMappingEvent				m_actionEvent;

			void	Activate(InputMappingID activatedMappingId)
			{
				m_actionEvent.Broadcast(activatedMappingId);
			}
		};

		typedef	HashMap<HashString, ActionDescriptor>	ActionMappings;


	public:

		/*
			Maps a keyboard event to the given action mapping. Creates a new one if it doesn't exist yet.
			Returns a keyboard mapping ID useful to identify this mapping.
		*/
		InputMappingID	BindActionMappingKeyboardEvent(const HashString& mappingName, const KeyboardMappingDesc& mappingDesc);


		Error	RegisterWindowRawInputDevices(RawInputHandler::WindowHandle windowForRawInput)
		{
			return m_rawInputHandler.RegisterWindowRawInputDevices(windowForRawInput);
		}

		Error	UnregisterWindowRawInputDevices()
		{
			return m_rawInputHandler.UnregisterWindowRawInputDevices();
		}



		void		HandleKeyDown(Keycode key);
		void		HandleKeyUp(Keycode key);


		void		AddActionMappingDelegate(const HashString& mappingName, const ActionMappingDelegate& dlgt);
		void		AddActionMappingDelegate(const HashString& mappingName, ActionMappingDelegate&& dlgt);


		bool		HasActionMapping(const HashString& mappingName) const;

		// Activate the action mapping externally. Will pass the action mapping ID as parameter to listener delegates.
		bool		ActivateActionMapping(const HashString& mappingName);


		ActionMappings::SizeType	NumActionMappings() const
		{
			return m_actionMappings.Size();
		}


		InputEventSink&	ModifyEventSink()
		{
			return m_eventSink;
		}


	private:

		bool	HandleKeyboardEvent(const KeyboardEventDesc& eventDesc);


		InputMappingID	m_nextID = 0;
		ActionMappings	m_actionMappings;

		RawInputHandler	m_rawInputHandler;

		InputEventSink	m_eventSink;

	};
}