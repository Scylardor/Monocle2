// Monocle Game Engine source files - Alexandre Baron

#include "catch.hpp"

 // TODO Fix compilation for now because missing raw input handler, but this has to be redone properly.
#if defined(MOE_WINDOWS) && defined(MOE_USE_WIN32)

#include "Input/InputHandler/InputHandler.h"


static int test = 0;

void testDelegate(moe::InputMappingID)
{
	++test;
}


TEST_CASE("Input", "[Input]")
{
	SECTION("Basic action mapping")
	{
		moe::HashString actionName("TestKeyboardMapping");
		moe::IInputHandler handler;

		handler.AddActionMappingDelegate(actionName, moe::IInputHandler::ActionMappingDelegate(&testDelegate));
		CHECK(handler.NumActionMappings() == 1);

		bool activated = handler.ActivateActionMapping(actionName);
		CHECK(activated);
		CHECK(test == 1);

		activated = handler.ActivateActionMapping(moe::HashString("NoName"));
		CHECK(false == activated);
	}


    SECTION("Keyboard mappings")
    {
		test = 0;

		moe::IInputHandler handler;

		CHECK(handler.NumActionMappings() == 0);

		moe::KeyboardMappingDesc description(moe::KEYBOARD_F1, moe::ButtonState::Pressed);

		moe::HashString actionName("TestKeyboardMapping");

		handler.BindActionMappingKeyboardEvent(actionName, description);
		CHECK(handler.NumActionMappings() == 1);



    }

}

#endif