#pragma once
#include "model.h"
class Cylinder : public Model {
public:
	Cylinder(void);
	~Cylinder(void);
	Cylinder* init();
	void update();
	void draw();
	void release();
	D3DXVECTOR3 pos();
	void setPos(const D3DXVECTOR3& pos);
private:
	D3DXVECTOR3 _pos;
	D3DXMATRIX world;
	LPD3DXCONSTANTTABLE vs_constant_table;
	LPD3DXCONSTANTTABLE ps_constant_table;
	LPDIRECT3DVERTEXSHADER9 vertex_shader;
	LPDIRECT3DPIXELSHADER9 pixel_shader;
	LPDIRECT3DTEXTURE9 texture;
};

