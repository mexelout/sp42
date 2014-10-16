#pragma once
#include "scene.h"
#include <d3dx9.h>
#include "ShaderPack.h"
#include <array>

class Player;
class StaticPlane;

class PointLightScene : public Scene {
public:
	PointLightScene(void);
	~PointLightScene(void);
	PointLightScene* init();
	void update();
	void draw();
	void release();
private:

	Player* player;
	StaticPlane* plane;

	D3DXVECTOR3 camera_rot;

	ShaderPack vs_point_light;
	ShaderPack ps_point_light;

	std::array<D3DXVECTOR3, 2> point_light_pos;
};

