#include "StaticPlane.h"
#include "ShaderDevise.h"
#include "Vertex.h"
#include "Camera.h"
#include "Common.h"

StaticPlane::StaticPlane(void) {
}

StaticPlane::~StaticPlane(void) {
}

StaticPlane* StaticPlane::init() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();

	device->CreateVertexBuffer(
		sizeof(CUSTOMVERTEX)*4,
		D3DUSAGE_WRITEONLY,
		D3DFVF_CUSTOMVERTEX,
		D3DPOOL_MANAGED,
		&vtx,
		NULL
		);

	float size = 10;
	CUSTOMVERTEX vtx[] = {
		CUSTOMVERTEX(D3DXVECTOR3(-size, 0,  size), D3DXVECTOR3(0, 1, 0), 0xffffffff, D3DXVECTOR2(0, 0)),
		CUSTOMVERTEX(D3DXVECTOR3( size, 0,  size), D3DXVECTOR3(0, 1, 0), 0xffffffff, D3DXVECTOR2(1, 0)),
		CUSTOMVERTEX(D3DXVECTOR3(-size, 0, -size), D3DXVECTOR3(0, 1, 0), 0xffffffff, D3DXVECTOR2(0, 1)),
		CUSTOMVERTEX(D3DXVECTOR3( size, 0, -size), D3DXVECTOR3(0, 1, 0), 0xffffffff, D3DXVECTOR2(1, 1))
	};

	D3DXVec3Cross(&vtx[0].nor, &(vtx[1].pos - vtx[0].pos), &(vtx[2].pos - vtx[0].pos));
	D3DXVec3Cross(&vtx[3].nor, &(vtx[2].pos - vtx[3].pos), &(vtx[1].pos - vtx[3].pos));
	D3DXVec3Normalize(&vtx[0].nor, &vtx[0].nor);
	D3DXVec3Normalize(&vtx[3].nor, &vtx[3].nor);

	void *data;
	this->vtx->Lock(0, sizeof(CUSTOMVERTEX)*4, &data, 0);
	memcpy(data, vtx, sizeof(CUSTOMVERTEX)*4);
	this->vtx->Unlock();
	return this;

}

void StaticPlane::update() {
}

void StaticPlane::draw() {
	LPDIRECT3DDEVICE9 device = ShaderDevise::device();
	device->SetFVF(D3DFVF_CUSTOMVERTEX);

	D3DXMATRIX world, view = Camera::view(), proj = Camera::projection();
	D3DXMatrixIdentity(&world);

	device->SetTransform(D3DTS_WORLD, &world);
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);

	device->SetStreamSource(0, vtx, 0, sizeof(CUSTOMVERTEX));
	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

}

void StaticPlane::release() {
	SAFE_RELEASE(vtx);
}

D3DXMATRIX StaticPlane::getWorld() {
	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);
	return world;
}

