#ifndef dx1_common_headers_h
#define dx1_common_headers_h

#include <d3dx9.h>

const int WW = 960;
const int WH = 640;
const int VPW = 960;
const int VPH = 640;

#define aspect_ratio (((float)VPW)/((float)VPH))

#define RED (D3DCOLOR_XRGB(255, 0, 0))
#define GREEN (D3DCOLOR_XRGB(0, 255, 0))
#define BLUE (D3DCOLOR_XRGB(0, 0, 255))
#define YELLOW (D3DCOLOR_XRGB(255, 255, 0))
#define WHITE (D3DCOLOR_XRGB(255, 255, 255))
#define BLACK (D3DCOLOR_XRGB(0, 0, 0))

#endif