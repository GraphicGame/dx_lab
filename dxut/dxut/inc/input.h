#ifndef input_h
#define input_h

#include "common_headers.h"

enum mouse_state {
	LBUTTON_DOWN = 0,
	LBUTTON_UP,
	LBUTTON_CLICK,
};

enum keyboard_state {
	KEY_DOWN = 0,
	KEY_UP,
	KEY_LAST
};

typedef void(*MOUSE_FUNC)(enum mouse_state state, int x, int y);
typedef void(*KEYBOARD_FUNC)(enum keyboard_state state, int code);

DXUTAPI void dxut_mouse_func(MOUSE_FUNC func);
DXUTAPI void dxut_keyboard_func(KEYBOARD_FUNC func);

#endif