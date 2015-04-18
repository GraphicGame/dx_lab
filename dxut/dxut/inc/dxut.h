#ifndef dxut_h
#define dxut_h

#include "common_headers.h"
#include "d3dx9.h"

typedef unsigned int uint;
typedef unsigned char uchar;
typedef void(*DRAW_FUNC)();

DXUTAPI bool dxut_init_d3d_window(uint w, uint h, const char *name, IDirect3DDevice9** device);
DXUTAPI void dxut_display_func(DRAW_FUNC func);
DXUTAPI void dxut_main_loop();

#endif