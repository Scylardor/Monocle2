
#include <Windows.h>

#include "Input/MonocleKeyboardMap.h"

namespace moe
{
	const Keycode KEYBOARD_0 = 0x30;
	const Keycode KEYBOARD_1 = 0x31;
	const Keycode KEYBOARD_2 = 0x32;
	const Keycode KEYBOARD_3 = 0x33;
	const Keycode KEYBOARD_4 = 0x34;
	const Keycode KEYBOARD_5 = 0x35;
	const Keycode KEYBOARD_6 = 0x36;
	const Keycode KEYBOARD_7 = 0x37;
	const Keycode KEYBOARD_8 = 0x38;
	const Keycode KEYBOARD_9 = 0x39;
	const Keycode KEYBOARD_A = 0x41;
	const Keycode KEYBOARD_B = 0x42;
	const Keycode KEYBOARD_C = 0x43;
	const Keycode KEYBOARD_D = 0x44;
	const Keycode KEYBOARD_E = 0x45;
	const Keycode KEYBOARD_F = 0x46;
	const Keycode KEYBOARD_G = 0x47;
	const Keycode KEYBOARD_H = 0x48;
	const Keycode KEYBOARD_I = 0x49;
	const Keycode KEYBOARD_J = 0x4a;
	const Keycode KEYBOARD_K = 0x4b;
	const Keycode KEYBOARD_L = 0x4c;
	const Keycode KEYBOARD_M = 0x4d;
	const Keycode KEYBOARD_N = 0x4e;
	const Keycode KEYBOARD_O = 0x4f;
	const Keycode KEYBOARD_P = 0x50;
	const Keycode KEYBOARD_Q = 0x51;
	const Keycode KEYBOARD_R = 0x52;
	const Keycode KEYBOARD_S = 0x53;
	const Keycode KEYBOARD_T = 0x54;
	const Keycode KEYBOARD_U = 0x55;
	const Keycode KEYBOARD_V = 0x56;
	const Keycode KEYBOARD_W = 0x57;
	const Keycode KEYBOARD_X = 0x58;
	const Keycode KEYBOARD_Y = 0x59;
	const Keycode KEYBOARD_Z = 0x5a;
	const Keycode KEYBOARD_ACCEPT = VK_ACCEPT;
	const Keycode KEYBOARD_ADD = VK_ADD;
	const Keycode KEYBOARD_APPS = VK_APPS;
	const Keycode KEYBOARD_ATTN = VK_ATTN;
	const Keycode KEYBOARD_BACK = VK_BACK;
	const Keycode KEYBOARD_BROWSER_BACK = VK_BROWSER_BACK;
	const Keycode KEYBOARD_BROWSER_FORWARD = VK_BROWSER_FORWARD;
	const Keycode KEYBOARD_CANCEL = VK_CANCEL;
	const Keycode KEYBOARD_CAPITAL = VK_CAPITAL;
	const Keycode KEYBOARD_CLEAR = VK_CLEAR;
	const Keycode KEYBOARD_CONTROL = VK_CONTROL;
	const Keycode KEYBOARD_CONVERT = VK_CONVERT;
	const Keycode KEYBOARD_CRSEL = VK_CRSEL;
	const Keycode KEYBOARD_DECIMAL = VK_DECIMAL;
	const Keycode KEYBOARD_DELETE = VK_DELETE;
	const Keycode KEYBOARD_DIVIDE = VK_DIVIDE;
	const Keycode KEYBOARD_DOWN = VK_DOWN;
	const Keycode KEYBOARD_END = VK_END;
	const Keycode KEYBOARD_EREOF = VK_EREOF;
	const Keycode KEYBOARD_ESCAPE = VK_ESCAPE;
	const Keycode KEYBOARD_EXECUTE = VK_EXECUTE;
	const Keycode KEYBOARD_EXSEL = VK_EXSEL;
	const Keycode KEYBOARD_F1 = VK_F1;
	const Keycode KEYBOARD_F10 = VK_F10;
	const Keycode KEYBOARD_F11 = VK_F11;
	const Keycode KEYBOARD_F12 = VK_F12;
	const Keycode KEYBOARD_F13 = VK_F13;
	const Keycode KEYBOARD_F14 = VK_F14;
	const Keycode KEYBOARD_F15 = VK_F15;
	const Keycode KEYBOARD_F16 = VK_F16;
	const Keycode KEYBOARD_F17 = VK_F17;
	const Keycode KEYBOARD_F18 = VK_F18;
	const Keycode KEYBOARD_F19 = VK_F19;
	const Keycode KEYBOARD_F2 = VK_F2;
	const Keycode KEYBOARD_F20 = VK_F20;
	const Keycode KEYBOARD_F21 = VK_F21;
	const Keycode KEYBOARD_F22 = VK_F22;
	const Keycode KEYBOARD_F23 = VK_F23;
	const Keycode KEYBOARD_F24 = VK_F24;
	const Keycode KEYBOARD_F3 = VK_F3;
	const Keycode KEYBOARD_F4 = VK_F4;
	const Keycode KEYBOARD_F5 = VK_F5;
	const Keycode KEYBOARD_F6 = VK_F6;
	const Keycode KEYBOARD_F7 = VK_F7;
	const Keycode KEYBOARD_F8 = VK_F8;
	const Keycode KEYBOARD_F9 = VK_F9;
	const Keycode KEYBOARD_FINAL = VK_FINAL;
	const Keycode KEYBOARD_KANA = VK_KANA;
	const Keycode KEYBOARD_KANJI = VK_KANJI;
	const Keycode KEYBOARD_HELP = VK_HELP;
	const Keycode KEYBOARD_HOME = VK_HOME;
	const Keycode KEYBOARD_INSERT = VK_INSERT;
	const Keycode KEYBOARD_JUNJA = VK_JUNJA;
	const Keycode KEYBOARD_LBUTTON = VK_LBUTTON;
	const Keycode KEYBOARD_LCONTROL = VK_LCONTROL;
	const Keycode KEYBOARD_LEFT = VK_LEFT;
	const Keycode KEYBOARD_LMENU = VK_LMENU;
	const Keycode KEYBOARD_LSHIFT = VK_LSHIFT;
	const Keycode KEYBOARD_LWIN = VK_LWIN;
	const Keycode KEYBOARD_MBUTTON = VK_MBUTTON;
	const Keycode KEYBOARD_MEDIA_NEXT_TRACK = VK_MEDIA_NEXT_TRACK;
	const Keycode KEYBOARD_MEDIA_PLAY_PAUSE = VK_MEDIA_PLAY_PAUSE;
	const Keycode KEYBOARD_MEDIA_PREV_TRACK = VK_MEDIA_PREV_TRACK;
	const Keycode KEYBOARD_MENU = VK_MENU;
	const Keycode KEYBOARD_MODECHANGE = VK_MODECHANGE;
	const Keycode KEYBOARD_MULTIPLY = VK_MULTIPLY;
	const Keycode KEYBOARD_NEXT = VK_NEXT;
	const Keycode KEYBOARD_NONAME = VK_NONAME;
	const Keycode KEYBOARD_NONCONVERT = VK_NONCONVERT;
	const Keycode KEYBOARD_NUMLOCK = VK_NUMLOCK;
	const Keycode KEYBOARD_NUMPAD0 = VK_NUMPAD0;
	const Keycode KEYBOARD_NUMPAD1 = VK_NUMPAD1;
	const Keycode KEYBOARD_NUMPAD2 = VK_NUMPAD2;
	const Keycode KEYBOARD_NUMPAD3 = VK_NUMPAD3;
	const Keycode KEYBOARD_NUMPAD4 = VK_NUMPAD4;
	const Keycode KEYBOARD_NUMPAD5 = VK_NUMPAD5;
	const Keycode KEYBOARD_NUMPAD6 = VK_NUMPAD6;
	const Keycode KEYBOARD_NUMPAD7 = VK_NUMPAD7;
	const Keycode KEYBOARD_NUMPAD8 = VK_NUMPAD8;
	const Keycode KEYBOARD_NUMPAD9 = VK_NUMPAD9;
	const Keycode KEYBOARD_OEM_CLEAR = VK_OEM_CLEAR;
	const Keycode KEYBOARD_PA1 = VK_PA1;
	const Keycode KEYBOARD_PAUSE = VK_PAUSE;
	const Keycode KEYBOARD_PLAY = VK_PLAY;
	const Keycode KEYBOARD_PRINT = VK_PRINT;
	const Keycode KEYBOARD_PRIOR = VK_PRIOR;
	const Keycode KEYBOARD_PROCESSKEY = VK_PROCESSKEY;
	const Keycode KEYBOARD_RBUTTON = VK_RBUTTON;
	const Keycode KEYBOARD_RCONTROL = VK_RCONTROL;
	const Keycode KEYBOARD_RETURN = VK_RETURN;
	const Keycode KEYBOARD_RIGHT = VK_RIGHT;
	const Keycode KEYBOARD_RMENU = VK_RMENU;
	const Keycode KEYBOARD_RSHIFT = VK_RSHIFT;
	const Keycode KEYBOARD_RWIN = VK_RWIN;
	const Keycode KEYBOARD_SCROLL = VK_SCROLL;
	const Keycode KEYBOARD_SELECT = VK_SELECT;
	const Keycode KEYBOARD_SEPARATOR = VK_SEPARATOR;
	const Keycode KEYBOARD_SHIFT = VK_SHIFT;
	const Keycode KEYBOARD_SNAPSHOT = VK_SNAPSHOT;
	const Keycode KEYBOARD_SPACE = VK_SPACE;
	const Keycode KEYBOARD_SUBTRACT = VK_SUBTRACT;
	const Keycode KEYBOARD_TAB = VK_TAB;
	const Keycode KEYBOARD_UP = VK_UP;
	const Keycode KEYBOARD_VOLUME_DOWN = VK_VOLUME_DOWN;
	const Keycode KEYBOARD_VOLUME_MUTE = VK_VOLUME_MUTE;
	const Keycode KEYBOARD_VOLUME_UP = VK_VOLUME_UP;
	const Keycode KEYBOARD_XBUTTON1 = VK_XBUTTON1;
	const Keycode KEYBOARD_XBUTTON2 = VK_XBUTTON2;
	const Keycode KEYBOARD_ZOOM = VK_ZOOM;
}
