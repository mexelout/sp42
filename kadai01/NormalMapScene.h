#pragma once
#include "scene.h"
#include <d3dx9.h>

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

};

