#pragma once

#include <array>
#include "Vertex.h"
#include "model.h"

class ToonModel : public Model {
public:
	ToonModel(void);
	~ToonModel(void);
	ToonModel* init();
	void update();
	void draw();
	void release();
	D3DXVECTOR3 pos();
	void calcGround(std::array<CUSTOMVERTEX, 4> vtx);
	void drawShadow();
private:
	D3DXVECTOR3 _pos;
	D3DXMATRIX world;
	LPD3DXCONSTANTTABLE vs_constant_table;
	LPD3DXCONSTANTTABLE vs_line_constant_table;
	LPD3DXCONSTANTTABLE ps_constant_table;
	LPDIRECT3DVERTEXSHADER9 vertex_shader;
	LPDIRECT3DVERTEXSHADER9 vertex_line_shader;
	LPDIRECT3DPIXELSHADER9 pixel_shader;
	LPDIRECT3DTEXTURE9 toon;

};

