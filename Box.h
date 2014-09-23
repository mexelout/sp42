#pragma once

#include <d3dx9.h>
#include <map>

class Model;

class Box {
public:
	Box();
	~Box();

	Box* init();
	void update();
	void draw();
	void release();

	D3DXMATRIX& getWorld();
	void setWorld(D3DXMATRIX& world);

	std::map<std::string, LPDIRECT3DTEXTURE9> getTextures();
private:
	Model* model;
	D3DXMATRIX world;
	LPDIRECT3DTEXTURE9 texture;
	LPDIRECT3DTEXTURE9 normal;
	LPDIRECT3DTEXTURE9 height_map;
};
