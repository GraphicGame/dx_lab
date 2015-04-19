#ifndef utils_h
#define utils_h

#include "common_headers.h"

DXUTAPI wchar_t* char2wchar_t(const char *str);
DXUTAPI char* wchar_t2char(const wchar_t *str);

DXUTAPI float deg_2_rad(float d);
DXUTAPI float rad_2_deg(float r);

#endif