#include "InputService.h"

#include "GameFramework/Service/TimeService/TimeService.h"
#include "GameFramework/Engine/Engine.h"

namespace moe
{

	InputService::InputService(Engine& ownerEngine) :
		Service(ownerEngine)
	{
	}


	void InputService::Update()
	{
		// Fire events for held keys
		for (Scancode heldScancode : m_heldKeysToNotify)
		{
			auto it = m_keyboardEvents.Find(heldScancode);
			MOE_ASSERT(it != m_keyboardEvents.End());
			MOE_DEBUG_ASSERT(it->second.OnHeld.IsBound()); // if no one listens to the held event we shouldn't be here
			it->second.OnHeld.Broadcast();
		}

		// Fire events for held buttons
		for (MouseButton button : m_heldButtonsToNotify)
		{
			auto it = m_mouseEvents.Find(button);
			MOE_ASSERT(it != m_mouseEvents.End());
			MOE_DEBUG_ASSERT(it->second.OnHeld.IsBound()); // if no one listens to the held event we shouldn't be here
			it->second.OnHeld.Broadcast();
		}


		auto* svcTime = GetEngine()->GetService< TimeService >();
		float dt = (svcTime ? svcTime->GetFrameDeltaTime() : 0);

		for (auto& controller : m_controllers)
		{
			controller->Update(dt);
		}

	}


	InputService::ButtonEvent& InputService::MutKeyActionEvent(Key k, ButtonPressedState state)
	{
		Scancode sc = glfwGetKeyScancode(k);

		// TryEmplace will find or insert default events in the map
		auto [it, _] = m_keyboardEvents.TryEmplace(sc);
		ButtonActionEvents& keyEvents = it->second;

		switch (state)
		{
		case ButtonPressedState::Pressed:
			return keyEvents.OnPress;
		case ButtonPressedState::Released:
			return keyEvents.OnReleased;
		case ButtonPressedState::Held:
			return keyEvents.OnHeld;
		}

		// not supposed to happen, but need to return something, anything...
		MOE_ASSERT(false);
		return keyEvents.OnPress;
	}


	InputService::ButtonEvent& InputService::MutMouseActionEvent(MouseButton button, ButtonPressedState state)
	{
		// TryEmplace will find or insert a default event in the map
		auto [it, _] = m_mouseEvents.TryEmplace(button);
		ButtonActionEvents& buttonEvents = it->second;

		switch (state)
		{
		case ButtonPressedState::Pressed:
			return buttonEvents.OnPress;
		case ButtonPressedState::Released:
			return buttonEvents.OnReleased;
		case ButtonPressedState::Held:
			// Someone is preparing to add a delegate to the held event of this button: monitor the key.
			m_heldButtonsToNotify.PushBack(button);
			return buttonEvents.OnHeld;
		}

		// not supposed to happen, but need to return something, anything...
		MOE_ASSERT(false);
		return buttonEvents.OnPress;
	}


	void InputService::AttachInputSource(InputSource& inputSrc)
	{
		m_attachedInputSource = &inputSrc;
		m_attachedInputSource->KeyboardKeyEvent.Add<InputService, &InputService::OnKeyEvent>(this);
		m_attachedInputSource->MouseButtonUpdateEvent.Add<InputService, &InputService::OnMouseEvent>(this);
	}


	void InputService::OnKeyEvent(int /*key*/, int scancode, ButtonPressedState action, int /*mods*/)
	{
		auto it = m_keyboardEvents.Find(scancode);
		if (it == m_keyboardEvents.End())
			return; // no one listens to that key!

		ButtonActionEvents& keyEvents = it->second;
		switch (action)
		{
		case ButtonPressedState::Pressed:
			// this key was just pressed: if someone wants to listen to its status while held, add it to the list
			if (keyEvents.OnHeld.IsBound())
				m_heldKeysToNotify.PushBack(scancode);

			keyEvents.OnPress.Broadcast();
			break;
		case ButtonPressedState::Released: {
			// this key was just released: if its held status was monitored, remove it from the list
			auto keyIt = std::find(m_heldKeysToNotify.Begin(), m_heldKeysToNotify.End(), scancode);
			if (keyIt != m_heldKeysToNotify.End())
				m_heldKeysToNotify.EraseBySwap(keyIt);

			keyEvents.OnReleased.Broadcast();
		}
			break;
		case ButtonPressedState::Held:
			keyEvents.OnHeld.Broadcast();
		}

	}

	void InputService::OnMouseEvent(MouseButton button, ButtonPressedState action, int /*mods*/)
	{
		// Depending on the button and the action, trigger the right event.
		auto it = m_mouseEvents.Find(button);
		if (it == m_mouseEvents.End())
			return; // no one listens to that button!

		ButtonActionEvents& buttonEvents = it->second;
		switch (action)
		{
		case ButtonPressedState::Pressed:
			// this button was just pressed: if someone wants to listen to its status while held, add it to the list
			if (buttonEvents.OnHeld.IsBound())
				m_heldButtonsToNotify.PushBack(button);

			buttonEvents.OnPress.Broadcast();
			break;
		case ButtonPressedState::Released: {
			// this button was just released: if someone listens to its status while held, remove it from the list
			auto buttonIt = std::find(m_heldButtonsToNotify.Begin(), m_heldButtonsToNotify.End(), button);
			if (buttonIt != m_heldButtonsToNotify.End())
				m_heldButtonsToNotify.EraseBySwap(buttonIt);

			buttonEvents.OnReleased.Broadcast();
			break;
		}
		case ButtonPressedState::Held:
			buttonEvents.OnHeld.Broadcast();
			break;
		}
	}
}
