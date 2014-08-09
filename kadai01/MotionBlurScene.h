#pragma once
#include "scene.h"
#include "ShaderPack.h"

class BlurPlayer;
class MeshField;

class MotionBlurScene : public Scene {
public:
	MotionBlurScene(void);
	~MotionBlurScene(void);
	MotionBlurScene* init();
	void update();
	void draw();
	void release();
private:
	BlurPlayer* player;
	MeshField* mesh_field;

	LPDIRECT3DTEXTURE9 texture;
	LPDIRECT3DSURFACE9 back_surface;
	LPDIRECT3DSURFACE9 z_surface;

	LPDIRECT3DSURFACE9 back_buffer;
	LPDIRECT3DSURFACE9 z_buffer;

	LPDIRECT3DVERTEXBUFFER9 vtx;

	ShaderPack vertex_shader;
	ShaderPack pixel_shader;
};

