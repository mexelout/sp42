#pragma once
#include <d3dx9.h>
#include "scene.h"
#include "ShaderPack.h"

class Box;
class Grid;

class NormalMapScene : public Scene {
public:
	NormalMapScene(void);
	~NormalMapScene(void);

	NormalMapScene* init();
	void update();
	void draw();
	void release();
private:

	Box* box;
	Grid* grid;
	D3DXVECTOR3 camera_rot;

	ShaderPack normal_vs;
	ShaderPack normal_ps;
};

