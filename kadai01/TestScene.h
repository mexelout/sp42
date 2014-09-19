#pragma once
#include "scene.h"
#include "ShaderPack.h"
#include <d3dx9.h>

class StonePavement;
class Player;

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

	D3DXVECTOR3 camera_rot;

	ShaderPack normal_vs;
	ShaderPack normal_ps;
};

