#pragma
#include <d3dx9.h>
#include "Scene.h"
#include "OwnTexture.h"
#include "ShaderPack.h"

class StaticPlane;
class Bokoboko;
class Grid;

#define SAMPLE_NUM 32

class SSAOScene : public Scene {
public:
	SSAOScene();
	~SSAOScene();
	SSAOScene* init();
	void update();
	void draw();
	void release();

private:
	LPDIRECT3DSURFACE9 bb;
	LPDIRECT3DVERTEXBUFFER9 vb;
	OwnTexture diffuse;
	OwnTexture depth;
	D3DXVECTOR3 camera_rot;

	Bokoboko* bkbk;
	StaticPlane* sp;
	Grid* grid;

	D3DXVECTOR2 rand_vec2[SAMPLE_NUM];

	UINT tex_number;
	ShaderPack ssao_vs;
	ShaderPack ssao_ps;
	ShaderPack ssao_light_ps;

};
// end file