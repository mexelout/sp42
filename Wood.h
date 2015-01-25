#pragma once

#include "Model.h"
#include <map>

class Wood : public Model{
public:
	Wood(void);
	~Wood(void);
	Wood* init();
	void update();
	void draw();
	void release();
	const D3DXMATRIX& world();
	void world(D3DXMATRIX& world);
	void setSamplerIndices(UINT si[3]);
private:
	D3DXMATRIX _world;
	UINT sampler_indices[3];
	LPDIRECT3DTEXTURE9 texture;
	LPDIRECT3DTEXTURE9 normal[2];
	LPDIRECT3DTEXTURE9 height_map[2];
};

