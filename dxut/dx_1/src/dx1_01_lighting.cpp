#define __exec

#include <stdio.h>
#include <assert.h>
#include <vector>
using namespace std;

#include "dx1_common_headers.h"
#include "dxut.h"
#include "input.h"
#include "utils.h"

static IDirect3DDevice9 *s_device = nullptr;
static IDirect3DVertexBuffer9 *s_vb = nullptr;
static IDirect3DIndexBuffer9 *s_ib = nullptr;
static D3DMATERIAL9 s_material;
static D3DLIGHT9 s_light_direction;

typedef struct vertex {
	vertex() {}
	vertex(float _x, float _y, float _z, int _idx) 
		:x(_x), y(_y), z(_z), idx(_idx)
	{
		
	}
	vertex(float _x, float _y, float _z, float _nx, float _ny, float _nz) 
		:x(_x), y(_y), z(_z), nx(_nx), ny(_ny), nz(_nz)
	{
	}

	float x, y, z;
	float nx, ny, nz;
	int idx;
	static const DWORD FVF;
} vertex;
const DWORD vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL;
static vector<vertex*> s_vertices;

typedef struct polygon {
	polygon() {}
	polygon(int _idx1, int _idx2, int _idx3)
		:idx1(_idx1), idx2(_idx2), idx3(_idx3)
	{
	}
	int idx1, idx2, idx3;
} polygon;
static vector<polygon*> s_polygons;

static float s_angle = 0.0f;

enum light_type {
	LT_DIRECTIONAL = 0,
	LT_POINT,
	LT_SPOT
};
static enum light_type s_current_light_type = LT_DIRECTIONAL;
static bool s_light_d_on = false;
static bool s_light_p_on = false;
static bool s_light_s_on = false;

static void setup_mat_view_projection() {
	D3DXMATRIX mat_v;
	D3DXVECTOR3 eye(0, 0, -2.0f);
	D3DXVECTOR3 at(0, 0, 1);
	D3DXVECTOR3 up(0, 1, 0);
	D3DXMatrixLookAtLH(&mat_v, &eye, &at, &up);
	s_device->SetTransform(D3DTS_VIEW, &mat_v);

	D3DXMATRIX mat_p;
	D3DXMatrixPerspectiveFovLH(&mat_p, deg_2_rad(60), aspect_ratio, 0.5f, 100.0f);
	s_device->SetTransform(D3DTS_PROJECTION, &mat_p);
}

static void compute_vertex_normal(vertex *pv) {
	vector<D3DXVECTOR3> normals;
	for (auto it = s_polygons.begin(); it != s_polygons.end(); it++) {
		polygon *pp = *it;
		if (pp->idx1 == pv->idx || pp->idx2 == pv->idx || pp->idx3 == pv->idx) {
			vertex *pp0 = s_vertices[pp->idx1];
			vertex *pp1 = s_vertices[pp->idx2];
			vertex *pp2 = s_vertices[pp->idx3];
			D3DXVECTOR3 p0(pp0->x, pp0->y, pp0->z);
			D3DXVECTOR3 p1(pp1->x, pp1->y, pp1->z);
			D3DXVECTOR3 p2(pp2->x, pp2->y, pp2->z);
			D3DXVECTOR3 u = p1 - p0;
			D3DXVECTOR3 v = p2 - p0;
			D3DXVECTOR3 n;
			D3DXVec3Cross(&n, &u, &v);
			D3DXVec3Normalize(&n, &n);
			normals.push_back(n);
		}
	}
	float nx = 0;
	float ny = 0;
	float nz = 0;
	for (auto it = normals.begin(); it != normals.end(); it++) {
		nx += (*it).x;
		ny += (*it).y;
		nz += (*it).z;
	}
	float sz = normals.size();
	pv->nx = nx / sz;
	pv->ny = ny / sz;
	pv->nz = nz / sz;
}

static void build_custom_vertices() {
	vertex *pv0 = new vertex(-1, 0, 1, 0);
	vertex *pv1 = new vertex(1, 0, 1, 1);
	vertex *pv2 = new vertex(0, 0, -1, 2);
	vertex *pv3 = new vertex(0, 2, 0, 3);
	s_vertices = { pv0, pv1, pv2, pv3 };

	polygon *pp0 = new polygon(0, 3, 2);
	polygon *pp1 = new polygon(2, 3, 1);
	polygon *pp2 = new polygon(1, 3, 0);
	polygon *pp3 = new polygon(0, 2, 1);
	s_polygons = { pp0, pp1, pp2, pp3 };

	for (auto it = s_vertices.begin(); it != s_vertices.end(); it++) {
		vertex *pv = *it;
		compute_vertex_normal(pv);
	}
}

static void setup_cube_vb_ib() {
	s_device->CreateVertexBuffer(
		4 * sizeof(vertex),
		D3DUSAGE_WRITEONLY,
		vertex::FVF,
		D3DPOOL_MANAGED,
		&s_vb,
		nullptr
		);
	s_device->CreateIndexBuffer(
		12 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&s_ib,
		nullptr);

	build_custom_vertices();

	vertex *vertices = nullptr;
	s_vb->Lock(0, 0, (void**)&vertices, 0);
	int idx = 0;
	for (auto it = s_vertices.begin(); it != s_vertices.end(); it++) {
		vertices[idx++] = **it;
	}
	s_vb->Unlock();

	idx = 0;
	WORD *indices = nullptr;
	s_ib->Lock(0, 0, (void**)&indices, 0);
	for (auto it = s_polygons.begin(); it != s_polygons.end(); it++) {
		polygon *pp = *it;
		indices[idx++] = pp->idx1;
		indices[idx++] = pp->idx2;
		indices[idx++] = pp->idx3;
	}
	s_ib->Unlock();
}

static void setup_rs() {
	s_device->SetRenderState(D3DRS_LIGHTING, true);
}

static void setup_material() {
	::ZeroMemory(&s_material, sizeof(s_material));
	s_material.Ambient = D3DXCOLOR(1, 0, 0, 1);
	s_material.Diffuse = D3DXCOLOR(1, 0, 0, 1);
	s_material.Specular = D3DXCOLOR(1, 0, 0, 1);
	s_material.Emissive = D3DXCOLOR(0, 0, 0, 1);
	s_material.Power = 5.0f;
}

static void setup_light() {
	::ZeroMemory(&s_light_direction, sizeof(s_light_direction));
	D3DXCOLOR c = D3DXCOLOR(1, 0, 0, 1);
	s_light_direction.Type = D3DLIGHT_DIRECTIONAL;
	s_light_direction.Ambient = c * 0.4f;
	s_light_direction.Diffuse = c;
	s_light_direction.Specular = c * 0.8f;
	s_light_direction.Direction = D3DXVECTOR3(0, 0, 1);
	
	s_device->SetLight(LT_DIRECTIONAL, &s_light_direction);
}

static void setup_dx() {
	assert(s_device != nullptr);
	setup_mat_view_projection();
	setup_cube_vb_ib();
	setup_material();
	setup_light();
	setup_rs();
}

static void on_draw() {
	s_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff00ff00, 1.0f, 0);

	D3DXMATRIX mat_t, mat_r, mat_w;
	D3DXMatrixRotationY(&mat_r, deg_2_rad(s_angle));
	D3DXMatrixTranslation(&mat_t, 0, 0, 2);
	mat_w = mat_r * mat_t;
	s_device->SetTransform(D3DTS_WORLD, &mat_w);
	if (++s_angle >= 360.0f) {
		s_angle = 0.0f;
	}

	s_device->BeginScene();
	s_device->SetStreamSource(0, s_vb, 0, sizeof(vertex));
	s_device->SetFVF(vertex::FVF);
	s_device->SetIndices(s_ib);
	s_device->SetMaterial(&s_material);
	s_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 4);
	s_device->EndScene();
	s_device->Present(0, 0, 0, 0);
}

static void on_key(keyboard_state ks, int code) {
	switch (code) {
	case 'D':
		if (ks == KEY_UP) {
			s_light_d_on = !s_light_d_on;
			s_device->LightEnable(LT_DIRECTIONAL, s_light_d_on);
		}
		break;
	case 'P':

		break;
	case 'S':

		break;
	default:
		break;
	}
}

#ifdef __exec
int main(int argc, char *argv[]) {
	dxut_init_d3d_window(WW, WH, "01", &s_device);

	setup_dx();

	dxut_display_func(on_draw);
	dxut_keyboard_func(on_key);
	dxut_main_loop();

	return 0;
}
#endif