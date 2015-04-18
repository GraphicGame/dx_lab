#include <stdio.h>

#include "dxut.h"
#include "input.h"

static IDirect3DDevice9 * s_device = nullptr;

static void on_key(keyboard_state ks, int key) {
	printf("on key...\n");
}

static void on_mouse(mouse_state ms, int x, int y) {
	printf("on mouse...\n");
}

int main(int argc, char *argv[]) {
	dxut_init_d3d_window(960, 640, "hello dxut!", &s_device);
	dxut_keyboard_func(on_key);
	dxut_mouse_func(on_mouse);
	dxut_main_loop();
	return 0;
}