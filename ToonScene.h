#pragma once
#include "scene.h"
#include "ShaderPack.h"

class ToonModel;

class ToonScene : public Scene {
public:
	ToonScene(void);
	~ToonScene(void);
	ToonScene* init();
	void update();
	void draw();
	void release();
private:
	ShaderPack toon_cell;
	ShaderPack toon_line;

	ToonModel* toon_model;
};

