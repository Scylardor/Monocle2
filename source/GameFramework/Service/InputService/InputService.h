#pragma once


#include <GLFW/glfw3.h>

#include "CameraControllers/FlyingCameraController.h"
#include "Core/Containers/HashMap/HashMap.h"
#include "Core/Containers/ObjectPool/ObjectPool.h"
#include "GameFramework/Service/Service.h"

#include "Monocle_GameFramework_Export.h"
#include "Core/Delegates/Event.h"

#include "InputSource.h"

#define MOE_KEY(k) GLFW_KEY_##k




namespace moe
{



	struct Monocle_GameFramework_API MouseButtonState
	{
		MouseButtonState() = default;

		MouseButtonState(MouseButton button, ButtonPressedState state) :
			Button(button), State(state)
		{}

		MouseButton			Button;
		ButtonPressedState	State;
	};


	struct Monocle_GameFramework_API KeyState
	{
		KeyState() = default;

		KeyState(int code, ButtonPressedState state) :
			Scancode(code), State(state)
		{}

		int					Scancode;
		ButtonPressedState	State;
	};




	class InputService : public Service
	{
	public:
		using ServiceLocatorType = InputService;

		InputService(Engine& ownerEngine);

		void	Update(float dt);

		using ButtonEvent = Event<void()>;

		using Key = int;

		[[nodiscard]] ButtonEvent& MutKeyActionEvent(Key k, ButtonPressedState state);


		using MouseButtonEvent = Event<void()>;
		using ButtonDelegate = MouseButtonEvent::DelegateType;

		[[nodiscard]] ButtonEvent& MutMouseActionEvent(MouseButton button, ButtonPressedState state);


		using MouseScrollEvent = Event<void(double, double)>;

		MouseScrollEvent& MutMouseScrollEvent()
		{
			MOE_ASSERT(m_attachedInputSource);
			return m_attachedInputSource->MouseScrollUpdateEvent;
		}


		using CursorPositionUpdateEvent = Event<void(double, double)>;

		CursorPositionUpdateEvent&	MutCursorUpdateEvent()
		{
			MOE_ASSERT(m_attachedInputSource);
			return m_attachedInputSource->CursorPositionUpdateEvent;
		}


		template <typename T, typename... Ts>
		T*	EmplaceController(Ts&&... args)
		{
			static_assert(std::is_base_of_v<IController, T>);

			m_controllers.EmplaceBack(std::make_unique<T>(std::forward<Ts>(args)...));
			return static_cast<T*>(m_controllers.Back().get());
		}


		void AttachInputSource(InputSource& inputSrc);

	private:


		void	OnKeyEvent(int key, int scancode, ButtonPressedState action, int mods);

		void	OnMouseEvent(MouseButton button, ButtonPressedState action, int mods);

		using Scancode = int;


		struct ButtonActionEvents
		{
			ButtonEvent	OnPress;
			ButtonEvent	OnReleased;
			ButtonEvent	OnHeld;
		};

		HashMap<Scancode, ButtonActionEvents>		m_keyboardEvents;
		HashMap<MouseButton, ButtonActionEvents>	m_mouseEvents;

		Vector<Scancode>	m_heldKeysToNotify;
		Vector<MouseButton>	m_heldButtonsToNotify;

		InputSource*		m_attachedInputSource = nullptr;

		Vector<std::unique_ptr<IController>>	m_controllers;
	};

}
