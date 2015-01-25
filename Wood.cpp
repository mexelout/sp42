#include "Wood.h"
#include "ShaderDevise.h"
#include "Vertex.h"
#include "Common.h"
#include "ModelManager.h"

Wood::Wood(void) {
	_world = Common::identity;
	normal[0] = NULL;
	normal[1] = NULL;
	height_map[0] = NULL;
	height_map[1] = NULL;
}
Wood::~Wood(void) {
}
Wood* Wood::init() {
	auto device = ShaderDevise::device();

	int wood_num = ModelManager::load("models/wood.x");
	Model* wood_model = ModelManager::get(wood_num);

	_mesh = wood_model->mesh();
	_num_materials = wood_model->numMaterials();
	_materials = wood_model->materials();
	_textures = wood_model->textures();

	D3DXCreateTextureFromFile(device, "textures/wood_normal.jpg", &normal[0]);
	D3DXCreateTextureFromFile(device, "textures/leaf_normal.jpg", &normal[1]);
	D3DXCreateTextureFromFile(device, "textures/wood_disp.jpg", &height_map[0]);
	D3DXCreateTextureFromFile(device, "textures/leaf_disp.jpg", &height_map[1]);

	return this;
}
void Wood::update() {
}
void Wood::draw() {
	auto device = ShaderDevise::device();

	D3DXMATERIAL* mat = (D3DXMATERIAL*)materials()->GetBufferPointer();
	int num_mat = numMaterials();
	device->SetFVF(D3DFVF_CUSTOMMODEL);
	LPD3DXMESH mesh = this->mesh();
	device->SetTransform(D3DTS_WORLD, &_world);

	D3DMATERIAL9 tmp_mat;
	device->GetMaterial(&tmp_mat);
	std::vector<LPDIRECT3DTEXTURE9> tex = textures();

	for(int i = 0; i < num_mat; i++) {
		device->SetMaterial(&mat[i].MatD3D);
		device->SetTexture(sampler_indices[0], tex[i]);
		device->SetTexture(sampler_indices[1], normal[i]);
		device->SetTexture(sampler_indices[2], height_map[i]);
		mesh->DrawSubset(i);
	}

	device->SetTexture(0, NULL);
	device->SetMaterial(&tmp_mat);
}
void Wood::release() {
	SAFE_RELEASE(normal[0]);
	SAFE_RELEASE(normal[1]);
	SAFE_RELEASE(height_map[0]);
	SAFE_RELEASE(height_map[1]);
}
const D3DXMATRIX& Wood::world() {
	return _world;
}
void Wood::world(D3DXMATRIX& world) {
	_world = world;
}
void Wood::setSamplerIndices(UINT si[3]) {
	for(int i = 0; i < 3; i++) {
		sampler_indices[i] = si[i];
	}
}

/*
		0.0000 200.0000 25.0000
		17.6777 200.0000 17.6777
		25.0000 200.0000 -0.0000
		17.6777 200.0000 -17.6777
		-0.0000 200.0000 -25.0000
		-17.6777 200.0000 -17.6777
		-25.0000 200.0000 0.0000
		-17.6777 200.0000 17.6777
		0.0000 0.0000 25.0000
		17.6777 0.0000 17.6777
		25.0000 0.0000 -0.0000
		17.6777 0.0000 -17.6777
		-0.0000 0.0000 -25.0000
		-17.6777 0.0000 -17.6777
		-25.0000 0.0000 0.0000
		-17.6777 0.0000 17.6777


*/
