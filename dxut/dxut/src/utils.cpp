#include "common_headers.h"
#include <windows.h>

#define M_PI 3.14159265358979323846

DXUTAPI wchar_t* char2wchar_t(const char* str) {
	int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	if (len == 0){
		return NULL;
	}
	wchar_t* buf = (wchar_t*)malloc(sizeof(wchar_t)*(len + 1));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, buf, len);

	return buf;
}

DXUTAPI char* wchar_t2char(const wchar_t* str) {
	int len = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
	if (len == 0){
		return NULL;
	}
	char* buf = (char*)malloc(sizeof(char)*(len + 1));
	WideCharToMultiByte(CP_UTF8, 0, str, -1, buf, len, NULL, NULL);

	return buf;
}

DXUTAPI float deg_2_rad(float d) {
	return d * (M_PI / 180);
}

DXUTAPI float rad_2_deg(float r) {
	return r * ( 180 / M_PI );
}