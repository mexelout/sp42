#pragma once
#include "scene.h"
#include "ShaderPack.h"

class GroundMesh;

class FogScene : public Scene {
public:
	FogScene(void);
	~FogScene(void);
	FogScene* init();
	void update();
	void draw();
	void release();
private:
	GroundMesh* ground_mesh;
	ShaderPack vs_linear;
	ShaderPack vs_length;
	ShaderPack vs_height;
};

