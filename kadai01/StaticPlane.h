#pragma once

#include <d3dx9.h>

class StaticPlane {
public:
	StaticPlane(void);
	~StaticPlane(void);
	StaticPlane* init();
	void update();
	void draw();
	void release();
	D3DXMATRIX getWorld();

protected:
	LPDIRECT3DVERTEXBUFFER9 vtx;


};

