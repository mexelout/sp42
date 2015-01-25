#pragma
#include "Scene.h"
#include "Sphere.h"
#include "ShaderPack.h"
#include "OwnTexture.h"

class MSAAScene : public Scene {
public:
	MSAAScene();
	~MSAAScene();
	MSAAScene* init();
	void update();
	void draw();
	void release();
private:
	Sphere* sphere;
	D3DXVECTOR3 camera_rot;

	OwnTexture z;
	LPDIRECT3DSURFACE9 dep;

	OwnTexture origin_z;
	LPDIRECT3DSURFACE9 origin_dep;

	ShaderPack msaa_vs_z;
	ShaderPack msaa_ps_z;
	ShaderPack msaa_vs;
	ShaderPack msaa_ps;

	LPDIRECT3DSURFACE9 bb;
	LPDIRECT3DSURFACE9 bdb;
	LPDIRECT3DVERTEXBUFFER9 vb;
};
// end file