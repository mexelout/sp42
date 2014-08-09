#pragma once
#include "scene.h"
#include "Vertex.h"
#include "BlurPlayer.h"
#include "StaticPlane.h"
#include "ShaderPack.h"

class MeshField;

class BlurScene : public Scene {
public:
	BlurScene(void);
	~BlurScene(void);
	BlurScene* init();
	void update();
	void draw();
	void release();
private:
	BlurPlayer* player;
	StaticPlane* plane;
	MeshField* mesh_field;
	//Plane* plane;


	LPDIRECT3DTEXTURE9 texture[4];
	LPDIRECT3DSURFACE9 back_surface[4];
	LPDIRECT3DSURFACE9 z_surface[4];

	LPDIRECT3DSURFACE9 back_buffer;
	LPDIRECT3DSURFACE9 z_buffer;

	LPDIRECT3DVERTEXBUFFER9 vtx[4];
	LPDIRECT3DVERTEXBUFFER9 vb;

	ShaderPack blur_vertical;
	ShaderPack blur_horizontal;
	ShaderPack depth_vertex;
	ShaderPack depth_pixel;

	ShaderPack multi_tex;
};

