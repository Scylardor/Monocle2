// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Input/InputHandler.h"

namespace moe
{
	bool IInputHandler::HandleKeyboardEvent(const KeyboardEventDesc& eventDesc)
	{
		bool handled = false;

		for (auto& actionMap : m_actionMappings)
		{
			ActionDescriptor& actionDesc = actionMap.second;

			for (KeyboardMapping& kbMap : actionDesc.m_kbMaps)
			{
				if (kbMap.Description().Matches(eventDesc))
				{
					actionDesc.Activate(kbMap.GetID());
					handled = true;
				}
			}
		}

		return handled;
	}


	moe::InputMappingID IInputHandler::BindActionMappingKeyboardEvent(const HashString& mappingName, const KeyboardMappingDesc& mappingDesc)
	{
		InputMappingID mappingID;

		ActionDescriptor& actionDesc = m_actionMappings[mappingName];
		actionDesc.m_kbMaps.EmplaceBack(m_nextID, mappingDesc);

		mappingID = m_nextID;
		m_nextID++;

		return mappingID;
	}


	void IInputHandler::HandleKeyDown(Keycode key)
	{

	}


	void IInputHandler::HandleKeyUp(Keycode key)
	{

	}


	bool IInputHandler::HasActionMapping(const HashString& mappingName) const
	{
		return (m_actionMappings.Find(mappingName) != m_actionMappings.End());
	}


	bool IInputHandler::ActivateActionMapping(const HashString& mappingName)
	{
		ActionMappings::Iterator mappingIt = m_actionMappings.Find(mappingName);
		if (mappingIt != m_actionMappings.End())
		{
			mappingIt->second.Activate(mappingName());
			return true;
		}

		return false;
	}


	void IInputHandler::AddActionMappingDelegate(const HashString& mappingName, const ActionMappingDelegate& dlgt)
	{
		ActionDescriptor& actionDesc = m_actionMappings[mappingName];
		actionDesc.m_actionEvent.AddDelegate(dlgt);
	}


	void IInputHandler::AddActionMappingDelegate(const HashString& mappingName, ActionMappingDelegate&& dlgt)
	{
		ActionDescriptor& actionDesc = m_actionMappings[mappingName];
		actionDesc.m_actionEvent.AddDelegate(std::move(dlgt));
	}

}