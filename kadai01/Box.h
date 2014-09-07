#pragma once

#include <d3dx9.h>

class Model;

class Box {
public:
	Box();
	~Box();

	Box* init();
	void update();
	void draw();
	void release();

private:
	Model* model;
	D3DXMATRIX world;

	LPDIRECT3DTEXTURE9 texture;
};
