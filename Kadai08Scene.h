#pragma
#include "Scene.h"
#include "ShaderPack.h"
#include "MutableArray.hpp"
#include <array>

class GroundMesh;
class Wood;
class Player;

class Kadai08Scene : public Scene {
public:
	Kadai08Scene();
	~Kadai08Scene();
	Kadai08Scene* init();
	void update();
	void draw();
	void release();
private:
	Player* player;

	GroundMesh* ground_mesh;
	std::array<Wood*, 300> woods;

	D3DXVECTOR3 camera_rot;

	ShaderPack stoplight_parallax_vs;
	ShaderPack stoplight_parallax_ps;
	ShaderPack stoplight_vs;
	ShaderPack stoplight_ps;
};
// end file