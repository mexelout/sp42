#pragma once

#include <d3dx9.h>

class OwnTexture {
public:
	OwnTexture();
	~OwnTexture();
	LPDIRECT3DTEXTURE9 tex;
	LPDIRECT3DSURFACE9 sur;
	void createTexture(UINT width, UINT height, UINT levels,DWORD usage, D3DFORMAT format, D3DPOOL pool);
};


