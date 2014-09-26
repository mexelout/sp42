#pragma once
#include "scene.h"
#include "ShaderPack.h"
#include <d3dx9.h>

class StonePavement;
class Player;
class Grid;
class Sphere;

class TestScene : public Scene {
public:
	TestScene(void);
	~TestScene(void);

	TestScene* init();
	void update();
	void draw();
	void release();

	StonePavement* stone_pavement;
	Player* player;
	Grid* grid;

	Sphere* sphere;

	D3DXVECTOR3 camera_rot;

	ShaderPack normal_vs;
	ShaderPack normal_ps;

	ShaderPack lambert_vs;
	ShaderPack lambert_ps;

	ShaderPack ppl_vs;
	ShaderPack ppl_ps;
};

