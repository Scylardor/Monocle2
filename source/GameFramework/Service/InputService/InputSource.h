#pragma once

#include <GLFW/glfw3.h>

namespace moe
{
	enum class MouseButton
	{
		LeftButton = GLFW_MOUSE_BUTTON_1,
		RightButton = GLFW_MOUSE_BUTTON_2,
		MiddleButton = GLFW_MOUSE_BUTTON_3,
		Button_0 = LeftButton,
		Button_1 = RightButton,
		Button_2 = MiddleButton,
		Button_3 = GLFW_MOUSE_BUTTON_3,
		Button_4 = GLFW_MOUSE_BUTTON_4,
		Button_5 = GLFW_MOUSE_BUTTON_5,
		Button_6 = GLFW_MOUSE_BUTTON_6,
		Button_7 = GLFW_MOUSE_BUTTON_7,
		Button_8 = GLFW_MOUSE_BUTTON_8,

	};


	enum class ButtonPressedState
	{
		Pressed = GLFW_PRESS,
		Held = GLFW_REPEAT,
		Released = GLFW_RELEASE
	};

	class InputSource
	{
	public:

		using KeyboardEvent = Event<void(int, int, ButtonPressedState, int)>;
		KeyboardEvent		KeyboardKeyEvent;

		using MouseButtonEvent = Event<void(MouseButton, ButtonPressedState, int)>;
		MouseButtonEvent	MouseButtonUpdateEvent;

		using MouseScrollEvent = Event<void(double, double)>;
		MouseScrollEvent	MouseScrollUpdateEvent;

		using CursorPositionEvent = Event<void(double, double)>;
		CursorPositionEvent	CursorPositionUpdateEvent;

		virtual void	PollInputs() = 0;
	};


}