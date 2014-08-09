#pragma once
#include "Scene.h"
#include "Vertex.h"
#include "DepthShadowPlayer.h"
#include "Plane.h"
#include "ShaderPack.h"

class DepthShadowScene : public Scene {
public:
	DepthShadowScene(void);
	~DepthShadowScene(void);
	DepthShadowScene* init();
	void update();
	void draw();
	void release();
private:
	DepthShadowPlayer* player;
	Plane* plane;

	LPDIRECT3DVERTEXBUFFER9 vtx;

	LPDIRECT3DTEXTURE9 texture;
	LPDIRECT3DSURFACE9 back_surface;
	LPDIRECT3DSURFACE9 z_surface;
	D3DVIEWPORT9       viewport;

	LPDIRECT3DSURFACE9 back_buffer;
	LPDIRECT3DSURFACE9 z_buffer;

	bool show_depth_shadow_texture;

	ShaderPack depth_vs;
	ShaderPack depth_ps;
	ShaderPack depth_shadow_vs;
	ShaderPack depth_shadow_ps;
};

