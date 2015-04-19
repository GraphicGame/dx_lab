#define __exec

#include <stdio.h>
#include <assert.h>

#include "dxut.h"
#include "utils.h"
#include "dx1_common_headers.h"
#include "input.h"

static IDirect3DDevice9 *s_device = nullptr;

static IDirect3DVertexBuffer9 *s_vb_triangle = nullptr;
static IDirect3DIndexBuffer9 *s_ib_triangle = nullptr;

static IDirect3DVertexBuffer9 *s_vb_cube = nullptr;
static IDirect3DIndexBuffer9 *s_ib_cube = nullptr;

enum primitive_type {
	PT_TRIANGLE = 0,
	PT_CUBE = 1
};
static enum primitive_type s_current_pt = PT_TRIANGLE;

float s_angle = 0.0f;

typedef struct vertex {
	vertex() {}
	vertex(float _x, float _y, float _z) 
		:x(_x), y(_y), z(_z)
	{
		
	}
	float x, y, z;
	static const DWORD FVF;
} vertex;
const DWORD vertex::FVF = D3DFVF_XYZ;

static void setup_viewport() {
	D3DVIEWPORT9 vp;
	vp.X = vp.Y = 0;
	vp.Width = VPW;
	vp.Height = VPH;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;
	s_device->SetViewport(&vp);
}

static void setup_triangle() {
	s_device->CreateVertexBuffer(
		3 * sizeof(vertex),
		D3DUSAGE_WRITEONLY,
		vertex::FVF,
		D3DPOOL_MANAGED,
		&s_vb_triangle,
		nullptr
		);
	s_device->CreateIndexBuffer(
		3 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&s_ib_triangle,
		nullptr
		);

	vertex *vertices;
	s_vb_triangle->Lock(0, 0, (void**)&vertices, 0);
	vertices[0] = vertex(0, 0, 0);
	vertices[1] = vertex(1, 0, 0);
	vertices[2] = vertex(0.5, 0.5, 0);
	s_vb_triangle->Unlock();

	WORD *indices;
	s_ib_triangle->Lock(0, 0, (void**)&indices, 0);
	indices[0] = 0;
	indices[1] = 2;
	indices[2] = 1;
	s_ib_triangle->Unlock();
}

static void setup_cube() {
	s_device->CreateVertexBuffer(
			8 * sizeof(vertex),
			D3DUSAGE_WRITEONLY,
			vertex::FVF,
			D3DPOOL_MANAGED,
			&s_vb_cube,
			nullptr
		);
	s_device->CreateIndexBuffer(
		36 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&s_ib_cube,
		nullptr
		);

	vertex *vertices = nullptr;
	s_vb_cube->Lock(0, 0, (void**)&vertices, 0);
	vertices[0] = vertex(-1, 1, 1);
	vertices[1] = vertex(1, 1, 1);
	vertices[2] = vertex(1, 1, -1);
	vertices[3] = vertex(-1, 1, -1);
	vertices[4] = vertex(-1, -1, 1);
	vertices[5] = vertex(1, -1, 1);
	vertices[6] = vertex(1, -1, -1);
	vertices[7] = vertex(-1, -1, -1);
	s_vb_cube->Unlock();

	WORD *indices = nullptr;
	s_ib_cube->Lock(0, 0, (void**)&indices, 0);
	WORD face_indices[6][6] = {
		{ 0, 1, 3, 3, 1, 2 },
		{ 3, 2, 7, 7, 2, 6 },
		{ 4, 7, 5, 5, 7, 6 },
		{ 0, 4, 1, 1, 4, 5 },
		{ 0, 3, 4, 4, 3, 7 },
		{ 2, 1, 6, 6, 1, 5 },
	};
	int i = 0;
	for (int row = 0; row < 6; row++) {
		for (int col = 0; col < 6; col++) {
			indices[i] = face_indices[row][col];
			++i;
		}
	}
	s_ib_cube->Unlock();
}

static void setup_mat_view() {
	D3DXVECTOR3 pos(0, 0, -2.0f);
	D3DXVECTOR3 target(0, 0, 1);
	D3DXVECTOR3 up(0, 1, 0);
	D3DXMATRIX mat_v;
	D3DXMatrixLookAtLH(&mat_v, &pos, &target, &up);
	s_device->SetTransform(D3DTS_VIEW, &mat_v);
}

static void setup_mat_projection() {
	D3DXMATRIX mat_p;
	D3DXMatrixPerspectiveFovLH(&mat_p, deg_2_rad(60), aspect_ratio, 1.0f, 100.0f);
	s_device->SetTransform(D3DTS_PROJECTION, &mat_p);
}

static void setup_dx() {
	assert(s_device != nullptr);
	setup_viewport();
	setup_triangle();
	setup_cube();
	setup_mat_view();
	setup_mat_projection();
	
	s_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	s_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
}

static void on_draw() {
	s_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff00ff00, 1.0f, 0);

	D3DXMATRIX mat_roty, mat_t, mat_world;
	D3DXMatrixRotationY(&mat_roty, deg_2_rad(s_angle));
	D3DXMatrixTranslation(&mat_t, 0, 0, 1.0f);
	mat_world = mat_roty * mat_t;
	s_device->SetTransform(D3DTS_WORLD, &mat_world);
	if (++s_angle >= 360.0f) {
		s_angle = 0.0f;
	}

	IDirect3DVertexBuffer9 *current_vb = nullptr;
	IDirect3DIndexBuffer9 *current_ib = nullptr;
	UINT num_vertex = 0;
	UINT prim_count = 0;
	switch (s_current_pt) {
	case PT_TRIANGLE:
		current_vb = s_vb_triangle;
		current_ib = s_ib_triangle;
		num_vertex = 3;
		prim_count = 1;
		break;
	case PT_CUBE:
		current_vb = s_vb_cube;
		current_ib = s_ib_cube;
		num_vertex = 8;
		prim_count = 12;
		break;
	default:
		break;
	}

	s_device->BeginScene();
	s_device->SetStreamSource(0, current_vb, 0, sizeof(vertex));
	s_device->SetFVF(vertex::FVF);
	s_device->SetIndices(current_ib);
	s_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, num_vertex, 0, prim_count);
	s_device->EndScene();

	s_device->Present(0, 0, 0, 0);
}

static void on_key(keyboard_state ks, int code) {
	switch (code) {
	case 'C':
		s_current_pt = PT_CUBE;
		break;
	case 'T':
		s_current_pt = PT_TRIANGLE;
		break;
	default:
		break;
	}
}

#ifdef __exec
int main(int argc, char *argv[]) {
	dxut_init_d3d_window(WW, WH, "1", &s_device);

	setup_dx();

	dxut_display_func(on_draw);
	dxut_keyboard_func(on_key);
	dxut_main_loop();

	return 0;
}
#endif