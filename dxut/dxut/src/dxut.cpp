#include "common_headers.h"
#include "dxut.h"
#include "utils.h"
#include "input.h"

#include <windows.h>
#include <d3dx9.h>
#include <assert.h>

#define WINDOW_CLASS_NAME L"DXUT"

static HWND s_hwnd;
static uint s_interval = 20;

static DRAW_FUNC s_on_draw = nullptr;
static MOUSE_FUNC s_on_mouse = nullptr;
static KEYBOARD_FUNC s_on_keyboard = nullptr;

static bool init_dx(HWND hwnd, uint w, uint h, IDirect3DDevice9 ** device) {
	HRESULT hr = 0;

	// Step 1: Create the IDirect3D9 object.
	IDirect3D9* d3d9 = nullptr;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d9) {
		::MessageBox(0, L"Direct3DCreate9() - FAILED", 0, 0);
		return false;
	}

	// Step 2: Check for hardware vp.
	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	int vp = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.
	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth = w;
	d3dpp.BackBufferHeight = h;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Step 4: Create the device.
	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		D3DDEVTYPE_HAL,         // device type
		hwnd,               // window associated with device
		vp,                 // vertex processing
		&d3dpp,             // present parameters
		device);            // return created device

	if (FAILED(hr)) {
		// try again using a 16-bit depth buffer
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

		hr = d3d9->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hwnd,
			vp,
			&d3dpp,
			device);

		if (FAILED(hr)) {
			d3d9->Release(); // done with d3d9 object
			::MessageBox(0, L"CreateDevice() - FAILED", 0, 0);
			return false;
		}
	}

	d3d9->Release(); // done with d3d9 object
	d3d9 = nullptr;
	return true;
}

static void get_mouse_xy(LPARAM lParam, int *x, int *y) {
	*x = (short)(lParam & 0xffff);
	*y = (short)((lParam >> 16) & 0xffff);
}

static LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
	case WM_CREATE:
		SetTimer(hwnd, 0, s_interval, NULL);
		break;
	case WM_TIMER:
		if (s_on_draw) {
			s_on_draw();
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		if (s_on_mouse) {
			int x, y;
			get_mouse_xy(lparam, &x, &y);
			enum mouse_state ms = (enum mouse_state)(msg - WM_LBUTTONDOWN);
			s_on_mouse(ms, x, y);
		}
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_KEYLAST:
		if (s_on_keyboard) {
			enum keyboard_state ks;
			switch (msg) {
			case WM_KEYDOWN:
				ks = KEY_DOWN;
				break;
			case WM_KEYUP:
				ks = KEY_UP;
				break;
			case WM_KEYLAST:
				ks = KEY_LAST;
				break;
			default:
				break;
			}
			s_on_keyboard(ks, wparam);
		}
		break;
	case WM_DESTROY:
		::CoUninitialize();
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

DXUTAPI bool dxut_init_d3d_window(uint w, uint h, const char *name, IDirect3DDevice9** device) {
	::CoInitialize(NULL);

	assert(w > 0 && w < 3000);
	assert(h > 0 && h < 3000);

	WNDCLASSEX winclass;
	HWND hwnd;

	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = window_proc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = GetModuleHandleW(0);
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&winclass))
		return false;

	wchar_t *w_name = char2wchar_t(name);
	hwnd = CreateWindowEx(NULL,
		WINDOW_CLASS_NAME,
		w_name,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		100, 100,
		w, h,
		NULL,
		NULL,
		GetModuleHandleW(0),
		NULL);
	free(w_name);
	if (hwnd == NULL)
		return false;
	s_hwnd = hwnd;

	if (!init_dx(hwnd, w, h, device)) {
		::MessageBox(0, L"init dx - FAILED", 0, 0);
		return false;
	}

	return true;
}

DXUTAPI void dxut_display_func(DRAW_FUNC func) {
	s_on_draw = func;
}

DXUTAPI void dxut_main_loop() {
	MSG msg;
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

DXUTAPI void dxut_mouse_func(MOUSE_FUNC func) {
	s_on_mouse = func;
}

DXUTAPI void dxut_keyboard_func(KEYBOARD_FUNC func) {
	s_on_keyboard = func;
}