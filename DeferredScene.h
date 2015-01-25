#pragma
#include <d3dx9.h>
#include "Scene.h"
#include "ShaderPack.h"
#include "OwnTexture.h"

class Model;
class Grid;
class Box;
class Sphere;

class DeferredScene : public Scene {
public:
	DeferredScene();
	~DeferredScene();
	DeferredScene* init();
	void update();
	void draw();
	void release();
private:
	OwnTexture diffuse;
	OwnTexture normal;
	D3DXVECTOR3 camera_rot;
	Box* m;
	Grid* g;
	LPDIRECT3DSURFACE9 bb;
	LPDIRECT3DVERTEXBUFFER9 vb;

	UINT sphere_tex_number;

	ShaderPack deferred_vs;
	ShaderPack deferred_ps;
	ShaderPack deferred_light_ps;
};
// end file