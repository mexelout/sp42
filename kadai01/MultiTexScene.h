#pragma once
#include "scene.h"
#include "Vertex.h"

class MultiTexScene : public Scene {
public:
	MultiTexScene(void);
	~MultiTexScene(void);
	MultiTexScene* init();
	void update();
	void draw();
	void release();

private:
	LPDIRECT3DTEXTURE9 texture1;
	LPDIRECT3DTEXTURE9 texture2;
	LPDIRECT3DVERTEXBUFFER9 vtx;
	LPD3DXCONSTANTTABLE ps_ct;
	LPDIRECT3DPIXELSHADER9 ps;
};

