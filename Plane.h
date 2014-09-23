#pragma once

#include <windows.h>
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <array>

#include "Vertex.h"

class Plane {
public:
	Plane(void);
	~Plane(void);
	Plane* init();
	void update();
	void draw();
	void release();
	std::array<CUSTOMVERTEX, 4> vertices();

private:
	LPDIRECT3DVERTEXBUFFER9 vbuf;

	D3DXMATRIX world;
	LPDIRECT3DTEXTURE9 texture;
	LPDIRECT3DTEXTURE9 white;
	std::array<CUSTOMVERTEX, 4> _vertices;
	float angle;

	D3DXHANDLE world_handle;
};

