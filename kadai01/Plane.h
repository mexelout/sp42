#pragma once

#include <windows.h>
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "Vertex.h"

class Plane {
public:
	Plane(void);
	~Plane(void);
	Plane* init(LPDIRECT3DDEVICE9 device);
	void update();
	void draw(LPDIRECT3DDEVICE9 device);
	void release();
private:
	LPDIRECT3DVERTEXBUFFER9 vbuf;
	LPD3DXCONSTANTTABLE _vs_constant_table;
	LPD3DXCONSTANTTABLE _ps_constant_table;
	LPDIRECT3DVERTEXSHADER9 _vs;
	LPDIRECT3DPIXELSHADER9 _ps;
	D3DXMATRIX world;
	LPDIRECT3DTEXTURE9 texture;
	float angle;
};

