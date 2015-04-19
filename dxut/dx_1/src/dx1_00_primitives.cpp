#define __exec

#include <stdio.h>
#include <assert.h>

#include "dxut.h"
#include "utils.h"
#include "dx1_common_headers.h"

static IDirect3DDevice9 *s_device = nullptr;
static IDirect3DVertexBuffer9 *s_vb_triangle = nullptr;
static IDirect3DIndexBuffer9 *s_ib_triangle = nullptr;

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
	vertices[0] = vertex(0, 0, 2);
	vertices[1] = vertex(0.5, 0, 2);
	vertices[2] = vertex(0.25, 0.2, 2);
	s_vb_triangle->Unlock();

	WORD *indices;
	s_ib_triangle->Lock(0, 0, (void**)&indices, 0);
	indices[0] = 0;
	indices[1] = 2;
	indices[2] = 1;
	s_ib_triangle->Unlock();
}

static void setup_mat_view() {
	D3DXVECTOR3 pos(0, 0, 0.9f);
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
	setup_mat_view();
	setup_mat_projection();
	
	s_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	s_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
}

static void on_draw() {
	s_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff00ff00, 1.0f, 0);

	D3DXMATRIX mat_world;
	D3DXMatrixTranslation(&mat_world, 0, 0, 0);
	s_device->SetTransform(D3DTS_WORLD, &mat_world);

	s_device->BeginScene();
	s_device->SetStreamSource(0, s_vb_triangle, 0, sizeof(vertex));
	s_device->SetFVF(vertex::FVF);
	s_device->SetIndices(s_ib_triangle);
	s_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1);
	s_device->EndScene();

	s_device->Present(0, 0, 0, 0);
}

#ifdef __exec
int main(int argc, char *argv[]) {
	dxut_init_d3d_window(WW, WH, "1", &s_device);

	setup_dx();

	dxut_display_func(on_draw);
	dxut_main_loop();
	return 0;
}
#endif