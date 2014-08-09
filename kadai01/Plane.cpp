
#include <string>
#include "Plane.h"
#include "Camera.h"
#include "ShaderDevise.h"
#include "Common.h"

Plane::Plane(void) {
	vbuf = NULL;
	texture = NULL;
	white = NULL;
	D3DXMatrixIdentity(&world);
}

Plane::~Plane(void) {
}

Plane* Plane::init() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	device->CreateVertexBuffer(
		sizeof(CUSTOMVERTEX)*4,
		D3DUSAGE_WRITEONLY,
		D3DFVF_CUSTOMVERTEX,
		D3DPOOL_MANAGED,
		&vbuf,
		NULL
		);

	CUSTOMVERTEX vtx[] = {
		CUSTOMVERTEX(D3DXVECTOR3(-10, 0,  10), D3DXVECTOR3(0, 1, 0), 0xffffffff, D3DXVECTOR2(0, 0)),
		CUSTOMVERTEX(D3DXVECTOR3( 10, 4,  10), D3DXVECTOR3(0, 1, 0), 0xffffffff, D3DXVECTOR2(1, 0)),
		CUSTOMVERTEX(D3DXVECTOR3(-10, 4, -10), D3DXVECTOR3(0, 1, 0), 0xffffffff, D3DXVECTOR2(0, 1)),
		CUSTOMVERTEX(D3DXVECTOR3( 10, 0, -10), D3DXVECTOR3(0, 1, 0), 0xffffffff, D3DXVECTOR2(1, 1))
	};

	D3DXVec3Cross(&vtx[0].nor, &(vtx[1].pos - vtx[0].pos), &(vtx[2].pos - vtx[0].pos));
	D3DXVec3Cross(&vtx[3].nor, &(vtx[2].pos - vtx[3].pos), &(vtx[1].pos - vtx[3].pos));
	D3DXVec3Normalize(&vtx[0].nor, &vtx[0].nor);
	D3DXVec3Normalize(&vtx[3].nor, &vtx[3].nor);

	for(int i = 0; i < 4; i++) 
		_vertices[i] = vtx[i];

	void *data;
	vbuf->Lock(0, sizeof(CUSTOMVERTEX)*4, &data, 0);
	memcpy(data, vtx, sizeof(CUSTOMVERTEX)*4);
	vbuf->Unlock();

	D3DXCreateTextureFromFile(device, "textures/shadow.png", &texture);
	D3DXCreateTextureFromFile(device, "textures/white.png", &white);

	angle = (D3DX_PI / 180.0f * 2);

	return this;
}

void Plane::update() {
}

void Plane::draw() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();

	device->SetFVF(D3DFVF_CUSTOMVERTEX);

	D3DXMATRIX rot;
	D3DXMatrixRotationAxis(&rot, &D3DXVECTOR3(0, 1, 0), angle);
	//world *= rot;
	D3DXMatrixIdentity(&world);

	//device->SetTexture(0, white);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	device->SetStreamSource(0, vbuf, 0, sizeof(CUSTOMVERTEX));
	device->SetTransform(D3DTS_WORLD, &world);
	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	/*
	// 投影テクスチャシャドウ用
	device->SetTexture(0, texture);
	device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	device->SetVertexShader(_vs_shadow);
	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	*/
	
	//device->SetTexture(0, NULL);
}

void Plane::release() {
	SAFE_RELEASE(vbuf);
	SAFE_RELEASE(texture);
	SAFE_RELEASE(white);
}

std::array<CUSTOMVERTEX, 4> Plane::vertices() {
	return _vertices;
}
