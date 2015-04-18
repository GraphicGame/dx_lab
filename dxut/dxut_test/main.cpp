#include <stdio.h>

#include "dxut.h"
#include "input.h"

static IDirect3DDevice9 * s_device = nullptr;

static void on_draw() {
	s_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x000ff000, 1.0f, 0);
	s_device->Present(0, 0, 0, 0);
}

static void on_key(keyboard_state ks, int key) {
	printf("on key...\n");
}

static void on_mouse(mouse_state ms, int x, int y) {
	printf("on mouse...\n");
}

int main(int argc, char *argv[]) {
	dxut_init_d3d_window(960, 640, "hello dxut!", &s_device);
	dxut_display_func(on_draw);
	dxut_keyboard_func(on_key);
	dxut_mouse_func(on_mouse);
	dxut_main_loop();
	return 0;
}